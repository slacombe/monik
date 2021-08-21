#include <sys/types.h>

#include <time.h>

#ifdef DEBUG
#include "debug.h"
#endif

#include "chess.h"
#include "quies.h"
#include "board.h"
#include "genmove.h"
#include "eval.h"
#include "make.h"
#include "unmake.h"
#include "logger/log.h"
#include "utile.h"
#include "hung.h"
#include "next.h"
#include "attaque.h"

extern int LeTemps;
extern int iTempRestants;
extern int iNodes;
extern int iVerifierTemp;
extern int maxply;
extern int maxQuiet;

// Algorithme de recherche MinMax avec des coupes Alpha-Beta.
// Ici on considere seulement les prises.
int Quiescence(int ply, int wtm, int alpha, int beta )
{
  int Valeur, AlphaInitiale;

  if ( ply >= MAXPLY-1 )
    return beta;

  iNodes++;

  if ( wtm ) {
    Valeur = Eval(ply, wtm, alpha, beta);
  }
  else {
    Valeur = -Eval(ply, wtm, alpha, beta);
  }

  // Remplace si le score est egal au score pour la NULLE.
  if ( Valeur == DRAWSCORE )
    Valeur = DRAWSCORE+1;

  AlphaInitiale = alpha;

  if ( Valeur > alpha ) {
    if ( Valeur >= beta ) return Valeur;
    alpha = Valeur;
    pv_length[ply] = ply-1;
  }

  // On genere toutes les prises.
  cb.MoveList[ply].nbmove = 0;
  GenMoveAttaque(ply, wtm, cb.MoveList[ply]);

  int keep = -1;
  // Le materiel.
  int iScoreMateriel = cb.ScoreMaterielBlanc-cb.ScoreMaterielNoir;
  iScoreMateriel = wtm?iScoreMateriel:-iScoreMateriel;
  int delta = alpha-100-iScoreMateriel;
  int iScoreGain;
  for( int i=0; i<cb.MoveList[ply].nbmove; i++ ) {
    // Garder le coup.
    bool bGarde = false;

    // On cherche seulement les coups qui ramene le materiel proche de alpha.
    // ex. Si celui qui jouent vient de se faire prendre une dame et que alpha
    // dit que le meilleur score est la perte d'un pion et bien ignorer tout
    // les captures qui ramene pas au moins la dame.
    if ( ValeurPiece[cb.MoveList[ply].moves[i].Capture] >= delta ) {
		if (cb.MoveList[ply].moves[i].Capture == ROI) return beta;
		
#ifdef USE_SEE
    	int iScoreCapture = ValeurPiece[cb.MoveList[ply].moves[i].Capture];
    	iScoreGain = iScoreCapture-ValeurPiece[cb.MoveList[ply].moves[i].Piece];
		if ((iScoreGain > 0 || iScoreGain >= 0) && delta <= 0) {
				bGarde = true;
		}
		else {
			iScoreGain = Echange(cb.MoveList[ply].moves[i].From, cb.MoveList[ply].moves[i].To, wtm );

      		if ( iScoreGain >= 0 ) {
        		bGarde = true;
    		}
		}
#else
		bGarde = true;
#endif
	}
	

    if ( bGarde ) {
      keep++;
      cb.MoveList[ply].moves[i].Score = iScoreGain;
      memcpy( &cb.MoveList[ply].moves[keep],
              &cb.MoveList[ply].moves[i],
              sizeof( TMove ) );
    }
  }
  cb.MoveList[ply].nbmove = keep+1;

  cb.MoveList[ply].currmove = -1;
  cb.MoveList[ply].Tri();
  Phase[ply] = CAPTURE_MOVES;
  // Maintenant, evaluer chaque coup.
  while( NextMove( cb, ply, wtm ) ) {

    // On execute le coup.
    MakeMove(ply, cb.MoveList[ply].CurrentMove(), wtm);
    // Mettre le coup dans le chemin actuel.
    cb.CurrentPath.moves[ply] = cb.MoveList[ply].moves[cb.MoveList[ply].currmove];
    if (!Check(wtm))
      Valeur = -Quiescence(ply+1, !wtm, -beta, -alpha);
    cb.MoveList[ply].CurrentMove().Score = Valeur;

    // On defait le coup.
    UnmakeMove(ply, cb.MoveList[ply].CurrentMove(), wtm);

    // Est-il meileur que notre valeur actuelle?
    if ( Valeur > alpha ) {
      if ( Valeur >= beta ) return Valeur;
	  pv_length[ply] = pv_length[ply+1];
	  pv[ply][ply] = cb.CurrentPath.moves[ply];
	  memcpy(&pv[ply][ply+1], &pv[ply+1][ply], sizeof(TMove)*(pv_length[ply]-ply));
      alpha = Valeur;
    }
#ifdef DEBUG
    Consistence( cb, cb.MoveList[ply].CurrentMove() );
#endif
  }

  return alpha;
}
