//---------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "journal.h"
#include "iteration.h"
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
#include "racine.h"
#include "system.h"
#include "book.h"

int prev_root_score, root_score, root_alpha, root_beta;

extern bool xboard;
extern TJournal journal;
int iNbCoups = 80;
int iEngTime = 30000;
int iOppTime = 30000;
int iMoveTime = 0;
int iNodes = 0;
int nbevals = 0;
int root_wtm;
int timeextension;
int timestamp;
int timeslot;
int timeabort;
int interrupted;
int pvsresearch;

int iProfondeurIteration;

//
// Cette routine appelle la negamax par iteration.
//
int Iteration(int wtm)
{
  g_iTranspositionHit = g_iTranspositionCollision = 0;
  g_iRefutation = 0;
  pvsresearch = 0;
  root_wtm = wtm;

  // On genere tous les coups pour ce noeud.
  cb.MoveList[1].nbmove = 0;
  GenMoveAttaque(1, wtm, cb.MoveList[1]);
  GenMovePasAttaque(1, wtm, cb.MoveList[1]);

  // On verifie si tous les coups sont valide.
  cb.MoveList[1].ChoisiMove(1, wtm);

  if (openingBook->inBook(cb.MoveList[1], wtm)) {
	return BOOKMOVE;
  }


  // Maintenant, executer chaque coup et l'evaluer pour lui donner un score de
  // depart.
  for( int i=0; i<cb.MoveList[1].nbmove; i++ ) {
    MakeMove(1, cb.MoveList[1].moves[i], wtm);
    if ( wtm ) {
      cb.MoveList[1].moves[i].Score = Eval(1, wtm, -INFINI, INFINI);
	  if ( cb.MoveList[1].moves[i].Capture ) {
		  cb.MoveList[1].moves[i].Score -= ValeurPiece[cb.MoveList[1].moves[i].Piece];
	  }
    }
    else {
      cb.MoveList[1].moves[i].Score = -Eval(1, wtm, -INFINI, INFINI);
	  if ( cb.MoveList[1].moves[i].Capture ) {
		  cb.MoveList[1].moves[i].Score -= ValeurPiece[cb.MoveList[1].moves[i].Piece];
	  }
    }
    UnmakeMove(1, cb.MoveList[1].moves[i], wtm);
  }

  // Trier le tout.
  cb.MoveList[1].Tri();

  // Le temps restant.
  timestamp = TempsCenti();

  // Nb de coups � faire pour ce temps.
  int nbCoups = cb.NoCoups/2;
  nbCoups = nbCoups % iNbCoups;
  int coupsRestants = iNbCoups - nbCoups;
  if (coupsRestants < 4) 
	  coupsRestants = 4;
  // La tranche de temps allou�e.
  if (iMoveTime != 0)
	timeslot = iMoveTime;
  else
	timeslot = iEngTime / coupsRestants;


#ifdef JOURNAL
  char Text[80];
  sprintf( Text, "iTempRestants: %d", iTempRestants );
  journal.Log( Text );
#endif

  // On creuse.
  iNodes = 0;
  nbevals = 0;
  iProfondeurIteration = 1;
  interrupted = false;
  timeextension = 0;
  timeabort = false;
  pv[1][1].Score = 0;
  char szContinuation[200];
  while( iProfondeurIteration < MAXPLY && (!timeabort || g_bModeAnalyse)) {

    cb.MoveList[1].currmove = -1;
//    printf( "\n" );
    // Si on est a la premiere iteration, grande fenetre.
    // Sinon, alpha = dernier score - 40 et beta = score + 40.
    // Si ce n'est pas le premier coup de la liste alors la fenetre
    // est retrecie a n et n+1.
    if ( iProfondeurIteration > 1 ) {
      root_alpha = prev_root_score - 40;
	  root_beta = prev_root_score + 40;
	  journal.Log( "root_alpha = %d, root_beta = %d", root_alpha, root_beta );
    }
    else {
      root_alpha = -INFINI;
      root_beta = INFINI;
    }

    root_score = SearchRacine(iProfondeurIteration, wtm, root_alpha, root_beta);
    if ( root_score >= root_beta ) {
	  // Stretch the beta limit.
      root_alpha = root_beta-1;
      root_beta = INFINI;

	  // Display the move that failed high.
	  SortieMove( cb.MoveList[1].CurrentMove(), szContinuation );

	  // Prepare to research the move.
	  cb.MoveList[1].currmove--;
	  if ( xboard ) {
		  if ( g_bModeAnalyse ) {
			printf( "%d %d %d %d %s!\n", iProfondeurIteration,
					root_score,
					(TempsCenti()-timestamp),
					iNodes, szContinuation );
		  }
		  else {
			printf( "%d %d %d %d %s!\n", iProfondeurIteration,
					root_score,
					(TempsCenti()-timestamp),
					iNodes, szContinuation );
		  }
	  }
	  else {
		printf( "\r [%d] (%2d/%d)  n: %8d  %5.2f        ++ %s!\n",
			iProfondeurIteration,
			cb.MoveList[1].currmove+2,
			cb.MoveList[1].nbmove,
			iNodes,	
			(TempsCenti()-timestamp)/100.0,
			szContinuation );
	  }
	  journal.Log( "[%2d] (%2d/%d) n: %8d  %5.2f        ++ %s!",
		iProfondeurIteration,
		cb.MoveList[1].currmove+2,
		cb.MoveList[1].nbmove,
		iNodes,
		(TempsCenti()-timestamp)/100.0,
		szContinuation );
	root_score = SearchRacine(iProfondeurIteration, wtm, root_alpha, root_beta);
    }
    else if ( root_score <= root_alpha && !timeabort && !interrupted ) {

	  // Stretch the alpha limit.
	  //root_beta = root_alpha+1;
	root_alpha = -INFINI;

	  // Display the move that failed low.
	  SortieMove( cb.MoveList[1].CurrentMove(), szContinuation );

	  // Research the move.
	cb.MoveList[1].currmove--;
	if ( xboard ) {
		printf( "%d %d %d %d %s?\n", iProfondeurIteration,
		root_score,
		(TempsCenti()-timestamp),
		iNodes, szContinuation );
	}
	else {
		printf( "\r [%d] (%2d/%d)  n: %8d  %5.2f       --  %s?\n",
			iProfondeurIteration,
			cb.MoveList[1].currmove+2,
			cb.MoveList[1].nbmove,
			iNodes,	
			(TempsCenti()-timestamp)/100.0,
			szContinuation );
	}
	journal.Log( "[%2d] (%2d/%d) n: %8d  %5.2f        -- %s?", iProfondeurIteration,
		cb.MoveList[1].currmove+2,
		cb.MoveList[1].nbmove,
		iNodes,
		(TempsCenti()-timestamp)/100.0,
		szContinuation );
      root_score = SearchRacine(iProfondeurIteration, wtm, root_alpha, root_beta);
    }

    if ( interrupted ) {
		char buf[4];
		int c = getc(stdin);
		journal.Log("Interrompu pour: %c", c);
		if (c == '.') {
			gets(buf);
		    char continuation[200];
			GetPV(continuation, &pv[1][1], iProfondeurIteration);
			printf( "%d %d %d %d %s\n", iProfondeurIteration, pv[1][1].Score,
                (TempsCenti() - timestamp),
       	        iNodes, continuation );
			journal.Log( "%d %d %d %d %s\n", iProfondeurIteration, pv[1][1].Score,
                (TempsCenti() - timestamp),
       	        iNodes, continuation);
			interrupted = false;
		}
		else {
			ungetc(c, stdin);
			break;
		}
	}

    if ( !g_bModeAnalyse && root_score >= MATE-50 ) {
      printf( "Mate\n" );
      break;
    }

    if ( !g_bModeAnalyse && root_score <= -MATE+50 ) {
      break;
    }

    if ( iProfondeurIteration >= 40 && root_score == 0 )
      break;

	if ( cb.MoveList[1].nbmove == 1 && iProfondeurIteration > 3 ) {
		break;
	}

	cb.MoveList[1].Tri();

    iProfondeurIteration++;
    prev_root_score = root_score;
  } // while

  iProfondeurIteration--;

  return pv[1][1].Score;
}
