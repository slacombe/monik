#include "search.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "eval.h"
#include "genmove.h"
#include "log.h"
#include "make.h"
#include "next.h"
#include "quies.h"
#include "repetition.h"
#include "system.h"
#include "time.h"
#include "unmake.h"
#include "utile.h"
#include "valide.h"
#include "stats.h"

#ifdef TRANSPOSITION
#include "transposition.h"
#endif

#ifdef DEBUG
#include "debug.h"
#endif

extern int iNodes;
extern int iMateInPly;
int iExtension;
extern int root_alpha, root_beta;
extern int iProfondeurIteration;
extern int timestop;
extern int timestamp;
extern int timeabort;
extern int interrupted;
extern int g_bCanAbort;

// Algorithme de recherche MinMax avec des coupes Alpha-Beta.
int search(TChessBoard *cb, int depth, int ply, int wtm, int alpha, int beta) {
  int Valeur, extension = 0, MoveCherche, danger = 0;
#ifdef TRANSPOSITION  
  int check_ext = 0;
  int old_alpha = alpha;
#endif

  if (ply >= MAXPLY - 1)
    return beta;

  iNodes++;

  // Si on n'est pas en mode Analyse, Reste-t-il du temps?
  if (interrupted || (timeabort && !g_bModeAnalyse))
    return beta;

  // Calculer le temps restant.
  if ((iNodes & 0xFF) == 0xFF) {
    if (TimeCheck())
      timeabort = true;
    // Verifier si il y a quelque chose dans le tampon
    // d'entree.

    if (inter()) {
      char buf[10];
      int c = fgetc(stdin);
      if (c == 10) {
        c = fgetc(stdin);
        ungetc(c, stdin);
        if (c == '.') {
          if (scanf("%s", buf) == 1) {
            printf("stat01: %d %d %d %d %d\n", (TempsCenti() - timestamp),
                   iNodes, iProfondeurIteration,
                   cb->MoveList[1].nbmove - cb->MoveList[1].currmove - 1,
                   cb->MoveList[1].nbmove);
            gameLog.log("received a dot\n");
          }
        } else {
          interrupted = true;
        }
      } if (c == 'x') {
        benchmark = false;
        interrupted = true;
      } else {
        ungetc(c, stdin);
        interrupted = true;
      }
    }
  }

  // Verifier si ce n'est pas une nulle.
  if (repetition(cb, wtm)) {
    return 0;
  }

  // Regarder dans la table de transposition pour voir si cette position
  // n'a pas deja ete calculer.
#ifdef TRANSPOSITION  
switch (lookup(cb, ply, depth, &alpha, &beta, &danger)) {
  case EXACT_SCORE:
    alphaBetaCutoffs++;
    Phase[ply] = HASH_MOVE;
    return alpha;
  case LOWER_BOUND:
    Phase[ply] = HASH_MOVE;
  case HASH_MISS:
    Phase[ply] = GENERATE_CAPTURE_MOVES;
    break;
  default:
    Phase[ply] = GENERATE_CAPTURE_MOVES;
}
#else
  Phase[ply] = GENERATE_CAPTURE_MOVES;
#endif

  // Maintenant, evaluer chaque coup.
  MoveCherche = 0;
  while (Phase[ply] != NO_MORE_MOVES && !interrupted) {
    if (nextMove(cb, ply, wtm)) {
      // On execute le coup.
      makeMove(cb, ply, currentMove(&cb->MoveList[ply]), wtm);
      // Mettre le coup dans le chemin actuel.
      cb->CurrentPath.moves[ply] =
          cb->MoveList[ply].moves[cb->MoveList[ply].currmove];

      if (!check(cb, wtm)) {
        MoveCherche++;

        // Si le coup met en echec etendre la recherche d'une profondeur
        // pour qu'il puisse en sortir.
        if (check(cb, !wtm)) {
          cb->inCheck[ply + 1] = true;
          extension = 1;
          cb->RaisonExtension[ply] = EXTENSION_ECHEC;
        } else {
          extension = 0;
          cb->inCheck[ply + 1] = false;
          cb->RaisonExtension[ply] = PAS_EXTENSION;
        }

        // Si le coup est une recapture
        // etendre la recherche d'une profondeur.
        if (cb->CurrentPath.moves[ply].Capture &&
            abs(ValeurPiece[cb->CurrentPath.moves[ply - 1].Capture] -
                ValeurPiece[cb->CurrentPath.moves[ply].Capture]) <= 20 &&
            (cb->CurrentPath.moves[ply - 1].To ==
             cb->CurrentPath.moves[ply].To) &&
            cb->RaisonExtension[ply - 1] != EXTENSION_RECAPTURE) {
          extension = 1;
          cb->RaisonExtension[ply] = EXTENSION_RECAPTURE;
        }

        // Si on est sur le point de promouvoir un pion, �tendre la recherche
        // pour voir si c'est une menace.
        if (extension == 0 && cb->CurrentPath.moves[ply].Piece == 1 &&
            PromoteExtension[cb->CurrentPath.moves[ply].To]) {
          extension = 1;
          cb->RaisonExtension[ply] = EXTENSION_PROMOTION;
        }

        // Si on pousse un pion passe, pousser la recherche plus
        // loin pour voir si c'est un danger.
        if (cb->CurrentPath.moves[ply].Piece == pion) {
          if (wtm) {
            if (cb->CurrentPath.moves[ply].To <= H5) {
              if (cb->PionPasseB[cb->CurrentPath.moves[ply].To & 7]) {
                extension = 1;
                cb->RaisonExtension[ply] = EXTENSION_PIONPASSE;
              }
            }
          } else if (cb->CurrentPath.moves[ply].To >= A4) {
            if (cb->PionPasseN[cb->CurrentPath.moves[ply].To & 7]) {
              extension = 1;
              cb->RaisonExtension[ply] = EXTENSION_PIONPASSE;
            }
          }
        }

        // Razoring trick. Idee prise dans Crafty.
        if (depth == 2 && !cb->inCheck[ply] && extension == 0) {
          int valeur;
          if (wtm)
            valeur = eval(cb, ply, wtm, alpha, beta);
          else
            valeur = -eval(cb, ply, wtm, alpha, beta);
          if (valeur + 50 < alpha)
            extension = -1;
        }

        // On l'explore.
        // Si c'est la variation principale, Fenetre normale, sinon,
        // fenetre est n et n+1.
        int inpv = 1;
        if (ply & 1) {
          if (alpha != root_alpha || beta != root_beta)
            inpv = 0;
        } else {
          if (alpha != -root_beta || beta != -root_alpha)
            inpv = 0;
        }
        if (inpv) {
          Valeur = -ABSearch(cb, depth - 1 + extension + danger, ply + 1, !wtm,
                             -beta, -alpha);
        } else {
          Valeur = -ABSearch(cb, depth - 1 + extension + danger, ply + 1, !wtm,
                             -alpha - 1, -alpha);
          if (Valeur > alpha && Valeur < beta) {
            pvsresearch++;
            Valeur = -ABSearch(cb, depth - 1 + extension + danger, ply + 1,
                               !wtm, -beta, -alpha);
          }
        }
      } else {
        Valeur = -INFINI;
      }

      setCurrentMoveScore(&cb->MoveList[ply], Valeur);

      // On defait le coup.
      unmakeMove(cb, ply, currentMove(&cb->MoveList[ply]), wtm);

      if (interrupted)
        return Valeur;

#ifdef DEBUG
      consistence(cb, ply, "search", currentMove(&cb->MoveList[ply]));
#endif
      // Est-il meilleur que notre valeur actuelle?
      if (Valeur > alpha) {
        if (Valeur >= beta) {
#ifdef TRANSPOSITION
          storeRefutation(cb, ply, depth, Valeur, check_ext);
          g_transpositionRefutation++;
#endif
          // Verifier si on peu l'utiliser comme killer move.
          if (Phase[ply] == NON_CAPTURE_MOVES) {
            addKiller(cb, cb->CurrentPath.moves[ply - 1], ply - 1);
          } else if (Phase[ply] == KILLER_MOVE_2) {
            cb->Killers[ply - 1][0].Score++;
          } else if (Phase[ply] == GENERATE_NON_CAPTURE_MOVES) {
            cb->Killers[ply - 1][1].Score++;
          }
          alphaBetaCutoffs++;
          if (Phase[ply] == KILLER_MOVE_1 || Phase[ply] == KILLER_MOVE_2) {
            killerMoveRefutationCount++;
          }

          return Valeur;
        }
        pv[ply][ply] = cb->CurrentPath.moves[ply];
        pv_length[ply] = pv_length[ply + 1];
        memcpy(&pv[ply][ply + 1], &pv[ply + 1][ply + 1],
               sizeof(TMove) * (pv_length[ply] - ply));
        alpha = Valeur;
      }
    } // if
  }

  // Verifier si il y a mat ou pat.
  if (MoveCherche == 0) {
    if (!check(cb, wtm)) {
      pv[ply][ply] = cb->CurrentPath.moves[ply];
      return 0;
      //		if ( wtm ) {
      //			return DRAWSCORE;
      //		}
      //		else {
      //			return -DRAWSCORE;
      //		}
    } else {
      if (ply < iMateInPly)
        iMateInPly = ply;
      alpha = -MATE + iMateInPly;
    }
  }

#ifdef TRANSPOSITION
  storeBest(cb, ply, depth, alpha, old_alpha, danger);
#endif

  return alpha;
}
