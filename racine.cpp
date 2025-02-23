#include <stdio.h>
#include <time.h>

#include "log.h"
#include "racine.h"
#include "board.h"
#include "genmove.h"
#include "make.h"
#include "unmake.h"
#include "search.h"
#include "sortie.h"
#include "eval.h"
#include "quies.h"
#include "hung.h"
#include "utile.h"
#include "transposition.h"
#include "chess.h"
#include "nextracine.h"
#include "system.h"
#include "time.h"

#ifdef DEBUG
#include "debug.h"
#endif

int iCoups = 0;
int maxply;
extern int iEngTime;
extern int iOppTime;
extern int iNodes;
int iVerifierTemp;
int iMateInPly;
extern int timestamp;
extern int timestop;
extern int timeabort;
extern int interrupted;
bool Post = true;

extern int root_alpha, root_beta;

extern bool xboard;
extern int iProfondeurIteration;

// Cette routine est la racine de l'arbre de recherche. Elle doit etre separee
// car plusieurs chose ne sont pas utile a la racine. Comme le null move et
// le lookup dans la table de transposition.
int searchRacine(TChessBoard *cb, int depth, int wtm, int alpha, int beta)
{
  int Valeur, extension = 0;

  // Calculer le temps restant.
  if ( (iNodes & 0xFF ) == 0xFF ) {
	  if ( TimeCheck() ) {
		  timeabort = true;
	  }
      // Verifier si il y a quelque chose dans le tampon
      // d'entree.
      if ( inter() ) {
        interrupted = true;
	  }
  }

  // Maintenant, evaluer chaque coup.
  while(nextMoveRacine(cb, wtm ) && !g_bAbort ) {

    if ( g_bModeAnalyse ) {
      printf( "stat01: %d %d %d %d %d\n", (TempsCenti()-timestamp),
              iNodes, iProfondeurIteration, cb->MoveList[1].nbmove - cb->MoveList[1].currmove-1,
              cb->MoveList[1].nbmove );
    }
    else if ( !xboard ) {
      printf( "\r [%d] (%2d/%d)", iProfondeurIteration, cb->MoveList[1].currmove+1,
               cb->MoveList[1].nbmove);
    }

    // On execute le coup.
	  iNodes++;
    makeMove(cb, 1, currentMove(&cb->MoveList[1]), wtm);
    // Mettre le coup dans le chemin actuel.
    cb->CurrentPath.moves[1] = cb->MoveList[1].moves[cb->MoveList[1].currmove];

    // Si le coup met en echec etendre la recherche d'une profondeur
    // pour qu'il puisse en sortir.
    if (check(cb, !wtm) ) {
      cb->inCheck[2] = true;
      extension = 1;
      cb->RaisonExtension[1] = EXTENSION_ECHEC;
    }
    else {
      extension = 0;
      cb->inCheck[2] = false;
      cb->RaisonExtension[1] = PAS_EXTENSION;
    }

    // On l'explore.
    // Si c'est la variation principale, Fenetre normale, sinon,
    // fenetre est n et n+1.
    if ( cb->MoveList[1].currmove == 0 ) {
      Valeur = -ABSearch(cb, depth-1+extension, 2, !wtm, -beta, -alpha);
    }
    else {
      Valeur = -ABSearch(cb, depth-1+extension, 2, !wtm, -alpha-1, -alpha);
      if ( Valeur > alpha && Valeur < beta ) {
        Valeur = -ABSearch(cb, depth-1+extension, 2, !wtm, -beta, -alpha);
      }
    }

    // La nouvelle valeur.
    cb->CurrentPath.moves[1].Score = Valeur;
    cb->MoveList[1].moves[cb->MoveList[1].currmove].Score = Valeur;

    // On defait le coup.
    unmakeMove(cb, 1, currentMove(&cb->MoveList[1]), wtm);

	if (interrupted)
		return Valeur;

#ifdef DEBUG
    consistence(cb, currentMove(&cb->MoveList[1]));
#endif
    // Est-elle meilleur que notre valeur actuelle?
    if (!timeabort || g_bModeAnalyse) {
      if (Valeur > alpha) {
        pv[1][1] = cb->CurrentPath.moves[1];
        pv_length[1] = pv_length[2];
        memcpy(&pv[1][2], &pv[2][2], sizeof(TMove) * (pv_length[1] - 1));
        affichePV(cb, iProfondeurIteration);
        if (Valeur >= beta) {
          return Valeur;
        }
        alpha = Valeur;
      } else if (cb->MoveList[1].currmove == 0)
        return alpha;
    }
  } // while

  return alpha;
}

void affichePV(TChessBoard *cb, int i_iProfondeur)
{
  pv_length[1] = i_iProfondeur;
  pv[1][1].Score = cb->CurrentPath.moves[1].Score;
  float fScore = pv[1][1].Score / 100.0;
  if ( xboard && Post ) {
    char szContinuation[255];
    getPV( szContinuation, &pv[1][1], pv_length[1] );
    if ( g_bModeAnalyse ) {
      printf( "%d %d %d %d %s\n", i_iProfondeur,
              pv[1][1].Score,
              (TempsCenti()-timestamp),
              iNodes, szContinuation );
    }
    else {
      printf( "%d %d %d %d %s\n", i_iProfondeur,
              pv[1][1].Score,
              (TempsCenti()-timestamp),
              iNodes, szContinuation );
    }
    gameLog.log( "[%2d] (%2d/%d) n: %8d  %5.2f   % 7.2f %s", i_iProfondeur,
                 cb->MoveList[1].currmove+1,
                 cb->MoveList[1].nbmove,
				 iNodes,
                 (TempsCenti()-timestamp)/100.0,
                 fScore,
                 szContinuation );
    }
    else if ( !xboard ) {
      char szContinuation[255];
      getPV( szContinuation, &pv[1][1], i_iProfondeur );
      printf( "\r [%d] (%2d/%d)  n: %8d %5.2f   % 7.2f %s\n", i_iProfondeur,
              cb->MoveList[1].currmove+1,
               cb->MoveList[1].nbmove,
				iNodes,	
               (TempsCenti()-timestamp)/100.0,
               fScore,
               szContinuation );
    }
}

