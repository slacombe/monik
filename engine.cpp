//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Engine.cpp
// Debut: 2 novembre 1998.
//
//---------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "chess.h"
#include "genmove.h"
#include "sortie.h"
#include "engine.h"
#include "edit.h"
#include "make.h"
#include "entree.h"
#include "utile.h"
#include "unmake.h"
#include "iteration.h"
#include "journal.h"
#include "repetition.h"
#include "system.h"
#include "search.h"

bool Moteur = false;    // true, tour moteur, false tour joueur.
bool Force = false;     // Mode force. Voir doc. winboard.
bool g_bEdit = false;      // Mode edit. Voir doc. winboard.
int whisper = true;
int wtm  = 1;

extern TJournal journal;
extern int iCoups;
extern int iNodes;
extern bool xboard;
extern int iEngTime;
extern int iMateInPly;
extern int iProfondeurIteration;
extern int timestamp;
extern int interrupted;

bool Engine( const char *i_szCommande, char* o_szReponse )
{
  // En mode d'édition.
  journal.Log("force = %d", Force);
  if ( g_bEdit ) {
    Edit( cb, i_szCommande, o_szReponse );
    journal.Log( "Edit command: %s", i_szCommande );
    return false;
  }
  
  // Verifier si la commande est une option.
  bool bOption = Option( i_szCommande, o_szReponse );
  if ( bOption ) {
    // La commande est une option.
    // Retourner directement la reponse.
    journal.Log( "Command: %s", i_szCommande );
    if (!g_bModeAnalyse)
            return false;
  }  

  TMove move;
  TMoveList movelist;
  if ( strcmp( i_szCommande, "undo" ) == 0 ) {
	UndoMove(); // Un coup de reculons.
  }
  else {
	  // Si c'est le tour du joueur on le joue sinon on passe
	  // au moteur.
	  if ( !Moteur || (Force && (strcmp(i_szCommande, "analyze") != 0))) {
	    if (wtm)
			journal.Log("Blanc (%d): %s", cb.NoCoups/2+1, i_szCommande);
	    else
			journal.Log("Noir (%d): %s", cb.NoCoups/2+1, i_szCommande);

	    // On prend le coup du joueur et on le transforme.
	    if (!Parse(i_szCommande, 0, wtm, move)) {
	      sprintf( o_szReponse, "Illegal move (parsing): %s", i_szCommande );
	      journal.Log("Illegal move (parsing): %s", i_szCommande);
	      return false;
	    }

	    // Verifier si le coup est valide de la facon suivante.
	    // Generer tout les coups possible du joueur et verifier si le
	    // coup est dans la liste.
	    // Ensuite, jouer le coup et verifier si le joueur n'est pas en echec.
	    GenMoveAttaque(1, wtm, movelist );
	    GenMovePasAttaque(1, wtm, movelist );
	    int iFound = -1;
	    for( int i=0; i<movelist.nbmove; i++ ) {
	      if ( movelist.moves[i].From == move.From &&
			  movelist.moves[i].To == move.To && movelist.moves[i].Promotion == move.Promotion ) {
		iFound = i;
		break;
	      }
	    }
	    if ( iFound == -1 ) {
	      sprintf( o_szReponse, "Illegal move: %s", i_szCommande );
	      journal.Log( "Illegal move: %s", i_szCommande);
	      return false;
	    }

	    // Remplacer par celui genere car il possede plus d'information.
	    move = movelist.moves[iFound];

	    MakeMove(0, move, wtm);
	    
	    if (Check(wtm)) {
	      UnmakeMove(0, move, wtm );
	      sprintf(o_szReponse, "Illegal move: %s", i_szCommande);
	      journal.Log("Coup illégale, met en echec: %s", i_szCommande);
	      return false;
	    }	
	    UnmakeMove(0, move, wtm);

	    MakeMoveRoot(move, wtm);

	    if ( Force ) {
	      printf( "%s\n", i_szCommande );
	    }
	//    if ( wtm )
	//      ChessBoard.History[iCoups][0] = move;
	//    else
	//      ChessBoard.History[iCoups++][1] = move;
	    wtm = !wtm;
            Moteur = !Moteur;
	  }
  }

  movelist.Vide();
  GenMoveAttaque(1, wtm, movelist);
  GenMovePasAttaque(1, wtm, movelist);
  movelist.ChoisiMove(1, wtm);
  if ( movelist.nbmove == 0 ) {
    if ( wtm ) {
		if ( Check(wtm) ) {
			strcpy( o_szReponse, "0-1 {Black mates}" );
			TSeeker::start();
		}
		else {
			strcpy( o_szReponse, "1/2-1/2 {Stalemate}" );
			TSeeker::start();
		}
    }
    else {
		if ( Check(wtm) ) {
	        strcpy( o_szReponse, "1-0 {White mates}" );
			TSeeker::start();
		}
		else {
	        strcpy( o_szReponse, "1/2-1/2 {Stalemate}" );
			TSeeker::start();
		}
    }
    journal.Log("%s", o_szReponse);
    return false;
  }

  // Verifier pour la nulle.
  if (RepetitionNulle(wtm)) {
    strcpy( o_szReponse, "1/2-1/2\n" );
    journal.Log("%s", o_szReponse);
    return false;
  }

  if ( !Force || g_bModeAnalyse ) {
	int Secondes;

    // C'est ici qu'on décole.
    // Logger le board 
    journal.logBoard();
    journal.Log("");

    // On prend en note le temps.
    timestamp = TempsCenti();

    // On démarre.
    int Score = Iteration(wtm);

    // Si le moteur retourne un score nulle alors verifier si
    // on a une partie nulle.
    if ( Score == 0 ) {
      if ( RepetitionNulle(wtm) ) {
        strcpy( o_szReponse, "result 1/2-1/2\n" );
		journal.Log("%s", o_szReponse);
		TSeeker::start();
        return false;
      }
    }

    if ( !g_bModeAnalyse ) {
      char szContinuation[255];
      GetPV( szContinuation, &pv[1][1], 1 );
      Secondes = (TempsCenti() - timestamp)/100;
      if ( Secondes < 1 )
        Secondes = 1;
      if ( xboard ) {
	  if ( Score == BOOKMOVE ) {
		char szBookInfo[2000];
		OutBookInfo( szBookInfo );
		printf( "%d %d %d %d %s\n", 
		  iProfondeurIteration, 
		  0,
		  (TempsCenti() - timestamp),
		  0, 
		  szBookInfo );
		float tempsPasse = TempsCenti() - timestamp;
		journal.Log("Book move - temps: %4.2f %s", tempsPasse, szBookInfo);
	}
	else {
		printf( "%d %d %d %d %s\n", iProfondeurIteration, pv[1][1].Score,
	                (TempsCenti() - timestamp),
        	        iNodes, szContinuation );
		journal.Log("Move - %d %d %d %d %s",
			iProfondeurIteration, pv[1][1].Score,
			(TempsCenti() - timestamp),
			iNodes, szContinuation);
	}
	if ( whisper ) {
		if ( Score == BOOKMOVE ) {
			char szBookInfo[2000];
			OutBookInfo( szBookInfo );
#ifdef		ICSTALK
			printf( "tellics whisper %s\n", szBookInfo );
#endif
		}
		else {
#ifdef		ICSTALK
			printf( "tellics whisper ply=%d; eval=%7.2f; nps=%dk\n",
				iProfondeurIteration, pv[1][1].Score / 100.0,
				(iNodes / Secondes)/1000 );
#endif
		}
	}
      }
      else {
        int Secondes = (TempsCenti() - timestamp)/100;
        iEngTime -= ( Secondes*100 );
        if ( Secondes < 1 )
          Secondes = 1;
        printf( "\n\nd: %d s: %7.2f nps: %d c: %s\n",
          iProfondeurIteration, pv[1][1].Score / 100.0,
          iNodes / Secondes,
          szContinuation );
	journal.Log("d: %d s: %3.2f nps: %d c: %s\n",
          iProfondeurIteration, 
	  pv[1][1].Score / 100.0,
          iNodes / Secondes,
          szContinuation );
      }
      MakeMoveRoot(pv[1][1], wtm);

      wtm = !wtm;

	  journal.Log( "\n\nTransposition hits: %d, collisions: %d\n", 
		g_iTranspositionHit, g_iTranspositionCollision );
	  journal.Log( "Refutation: %d\n", g_iRefutation );
	  journal.Log( "nps: %dk", (iNodes/Secondes)/1000 );
	  journal.Log( "eps: %dk", (nbevals/Secondes)/1000 );
      SortieMove( pv[1][1], o_szReponse );
      journal.Log("My move: %s", o_szReponse);

	  char szText[1000];
	  sprintf( szText, "pvsresearch: %d\n", pvsresearch );
	  journal.Log( szText );
    }
  }

  if (!g_bModeAnalyse)
	Moteur = !Moteur;
  
  if (Force) {
      return false;
  }

  return true;
}
