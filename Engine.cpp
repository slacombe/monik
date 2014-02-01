//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Engine.cpp
// Debut: 2 novembre 1998.
//
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "engine.h"

#include "chess.h"
#include "eval.h"
#include "genmove.h"
#include "sortie.h"
#include "make.h"
#include "entree.h"
#include "utile.h"
#include "unmake.h"
#include "iteration.h"
#include "journal.h"
#include "repetition.h"
#include "system.h"
#include "search.h"
#include "chargeur.h"
#include "Book.h"

bool Moteur = false;    // true, tour moteur, false tour joueur.
bool g_bEdit = false;      // Mode edit. Voir doc. winboard.
int whisper = true;
int wtm  = 1;
bool xboard = false;

extern TJournal journal;
extern int iCoups;
extern int iNbCoups;
extern int iNodes;
extern bool xboard;
extern int iEngTime;
extern int iMateInPly;
extern int iProfondeurIteration;
extern int timestamp;
extern int interrupted;
extern int iOppTime;
extern bool Post;
extern int iMoveTime;

Engine::Engine() : Mode(Play)
{
}

// Routine qui obtient une commande a partir de la console.
// Pour interfacer le moteur avec tout autre interface, on peut passer
// directement la fonction engine.
void Engine::Entree(char* o_szCommande)
{
	static char szText[255];

	// En mode xboard on n'affiche pas la demande de commande.
	if ( !xboard ) {
		// Les blancs ou les noirs.
		if ( wtm )
			sprintf( szText, "Blanc (%d): ", cb.NoCoups/2+1);
		else
			sprintf( szText, "Noir (%d): ", cb.NoCoups/2+1);

		printf( "%s", szText );
	}


	strcpy( o_szCommande, "" );
	do { 
		scanf( "%s", o_szCommande );
		journal.Log("entree: %s", o_szCommande);
	} while( strlen( o_szCommande ) == 0 );
}

bool Engine::Run( const char *i_szCommande, char* o_szReponse )
{
  journal.Log("force = %d", Mode);
  if ( Mode == EngineMode::Edit ) {
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
		return true;
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

  return true;
}

// Cette routine verifie si une commande est execute, l'execute et
// retourne true. Sinon elle retourne false pour que le moteur sache
// que c'est un coup.
bool Engine::Option( const char* i_szCommande, char* o_szReponse )
{
	strcpy( o_szReponse, "" );

	if ( strcmp( i_szCommande, "quit" ) == 0 ) {
		char szBoard[1000];
		char szText[10];
		strcpy( szBoard, "" );
		for( int i=A8; i<=H1; i++ ) {
			sprintf( szText, "%+2d", cb.board[i] );
			strcat( szBoard, szText );
			if ( i % 8 == 7 )
				strcat( szBoard, "\n" );
		}
		journal.Log( szBoard );
		return true;
	}

	if (strcmp(i_szCommande, "eval") == 0) {
		int score = Eval(1, wtm, -INFINI, INFINI);
		printf("score = %d\n", score);
		journal.Log("score = %d", score);
		return true;
	}

	if (strcmp(i_szCommande, "board") == 0) {
		for(int i=A8; i<=H1; i++) {
			printf(" %2d ", cb.board[i]);
			if (i % 8 == 7) 
				printf("\n");
		}
		return true;
	}

	// On verifie la commande.
	// On demande le mode xboard.
	// Il n'y a pas de possibilite de retourne au mode normal.
	if ( strcmp( i_szCommande, "xboard" ) == 0 ) {
		journal.Log( "Interface graphique: Winboard" );
		xboard = 1;
		return true;
	}

	// On demande une nouvelle partie.
	if ( strcmp( i_szCommande, "new" ) == 0 ) {
		cb.InitialiseBoard();
		cb.InitialiseBitboard();
		wtm = true;
		Moteur = false;
		Mode = EngineMode::Play;
		iEngTime = 5*60*100;  // 5 minutes par defaut.
		iOppTime = 5*60*100;
		return true;
	}

	// Le moteur est mis en mode edit.
	// Permet d'entrer une position specifique.
	if ( strcmp( i_szCommande, "edit" ) == 0 ) {
		journal.Log( "Edit." );
		g_bEdit = true;
		WhiteToMove = true;
		return true;
	}

	// Chargement d'une position.
	if ( strcmp( i_szCommande, "loadpos" ) == 0 ) {
		char szFichier[60];
		printf( "Entrez le nom de fichier: " );
		scanf( "%s", szFichier );
		if (ChargePosition( cb, szFichier))
			printf( "Chargement ok." );
		else
			printf( "Chargement pas reussi." );
		return true;
	}

	// Le moteur est mis en mode d'analyse.
	if ( strcmp( i_szCommande, "analyze" ) == 0 ) {
		journal.Log( "Analyse." );
		Moteur = true;
		g_bModeAnalyse = true;
		Mode = EngineMode::Force;
		return true;
	}

	// On demande une mise à jour d'analyse.
	if (strcmp(i_szCommande, ".") == 0) {
		if (g_bModeAnalyse)
			return true;
		else
			return false;
	}

	// Do whisper.
	if ( strcmp( i_szCommande, "dowhisper" ) == 0 ) {
		whisper = true;
	}

	// Do whisper.
	if ( strcmp( i_szCommande, "nowhisper" ) == 0 ) {
		whisper = false;
	}

	// Le moteur doit etre sortie du mode d'analyse.
	if ( strcmp( i_szCommande, "exit" ) == 0 ) {
		journal.Log( "Exit." );
		Moteur = false;
		Mode = EngineMode::Play;
		g_bModeAnalyse = false;
		return true;
	}

	// Le moteur joue le joueur courant.
	if ( strcmp( i_szCommande, "go" ) == 0 ) {
		journal.Nouveau();
		journal.Log( "Demarrage." );
		// Je sais que c'est une option et que je devrais retourner true, mais
		// je retourne false c'est pour forcer le moteur a jouer son coup.
#ifdef  ICSTALK
		printf("tellics set 1 %s", nomProgramme);
		printf("tellics say Hello from %s", nomProgramme );
#endif
		Moteur = true;
		Mode = EngineMode::Play;
		return false;
	}

	// Les blancs jouent les prochains.
	if ( strcmp( i_szCommande, "white" ) == 0 ) {
		journal.Log( "Les blancs jouent." );
		wtm = true;
		return true;
	}

	// Les noirs jouent les prochains.
	if ( strcmp( i_szCommande, "black" ) == 0 ) {
		journal.Log( "Les noirs jouent." );
		wtm = false;
		return true;
	}

	// Met le moteur avec un peu de random, pas implante.
	if ( strcmp( i_szCommande, "random" ) == 0 ) {
		return true;
	}

	// On set le level.
	if ( strcmp( i_szCommande, "level" ) == 0 ) {
		char szNbCoups[10], szNbMinute[10], szIncrement[10];
		scanf("%s", szNbCoups);
		scanf("%s", szNbMinute);
		scanf("%s", szIncrement);
		iNbCoups = atoi(szNbCoups);
		iEngTime = atoi(szNbMinute) * 60 * 100;
		if (iNbCoups == 0)
			iNbCoups = 80;
		return true;
	}

	// Show thinking.
	if ( strcmp( i_szCommande, "post" ) == 0 ) {
		Post = true;
		return true;
	}

	// No show thinking.
	if ( strcmp( i_szCommande, "nopost" ) == 0 ) {
		Post = false;
		return true;
	}

	// Le moteur peu penser sur le temps du joueur au prochain coup.
	if ( strcmp( i_szCommande, "hard" ) == 0 ) {
		printf( "(no pondering)\n" );
		return true;
	}

	// Le moteur ne pense que sur son temps.
	if ( strcmp( i_szCommande, "easy" ) == 0 ) {
		printf( "(no pondering)\n" );
		return true;
	}

	if (strcmp(i_szCommande, "st") == 0) {
		scanf("%d", &iMoveTime);
		journal.Log("move time = %d", iMoveTime);
		return true;
	}

	// Set l'horloge.
	if ( strcmp( i_szCommande, "time" ) == 0 ) {
		scanf( "%d", &iEngTime );
		char szTemp[20];
		sprintf( szTemp, "%d", iEngTime );
		journal.Log( "time = %d", iEngTime );
		return true;
	}

	// La deuxieme horloge.
	if ( strcmp( i_szCommande, "otim" ) == 0 ) {
		scanf( "%d", &iOppTime );
		journal.Log("otime = %d", iOppTime);
		return true;
	}

	if ( strcmp( i_szCommande, "name" ) == 0 ) {
		char szName[40];
		scanf( "%s", szName );
		journal.Log( "name = %s", szName );
		TSeeker::stop();
		return true;
	}

	if ( strcmp( i_szCommande, "rating" ) == 0 ) {
		int myrating, rating;
		scanf( "%d", &myrating );
		journal.Log( "my rating = %d", myrating );
		scanf( "%d", &rating );
		journal.Log( "Opponent rating = %d", rating );
		return true;
	}

	if ( strcmp( i_szCommande, "protover" ) == 0 ) {
		float version;
		scanf( "%f", &version );
		journal.Log( "proto version: %f\n", version );
		return true;
	}

	// Le moteur est mis off-line. Les prochains coups recus doit
	// etre verifier pour validite mais le moteur ne reponds pas par un
	// coup et ne pense pas non plus (no pondering)
	if ( strcmp( i_szCommande, "force" ) == 0 ) {
		Mode = EngineMode::Force;
		return true;
	}

	// La partie est finie, on recoit le resultat.
	// Pas implante.
	if ( strcmp( i_szCommande, "result" ) == 0 ) {
		char szTemp[255];
		fgets( szTemp, sizeof( szTemp ), stdin ); 
		TSeeker::start();
		return true;
	}

	//
	// Generation de la librairie d'ouverture de depart.
	//
	if ( strcmp( i_szCommande, "createbook" ) == 0 ) {
		char szText[255];
		scanf( "%s", szText );

		CreateStartBook( szText );
		return true;
	}

	return false;
}

void Engine::Edit( TChessBoard& cb, const char* i_szCommande, char* o_szReponse )
{
	const char* szCode = "PNBRQK.";

	strcpy( o_szReponse, "" );
	if (strcmp( i_szCommande, "c" ) == 0) {
		WhiteToMove = !WhiteToMove;
		return;
	}

	if (strcmp( i_szCommande, "#" ) == 0) {
		memset( &cb, 0, sizeof( cb ) );
		return;
	}

	// Sort du mode edit.
	if ( strcmp( i_szCommande, "." ) == 0 ) {
		g_bEdit = false;
		cb.InitialiseBitboard();
		return;
	}

	// Calculer le type de piece a ajouter.
	int iType = -1;
	for( unsigned int i=0; i<strlen( szCode ); i++ )
		if ( i_szCommande[0] == szCode[i] )
			iType = i+1;

	// Caractere valide?
	if ( iType == -1 )
		return;

	if ( !WhiteToMove )
		iType = -iType;

	// Calculer la position.
	int iColonne = i_szCommande[1]-'a';
	if ( iColonne < 0 || iColonne > 7 )
		return;
	int iRangee = i_szCommande[2]-'1';
	if ( iRangee < 0 || iRangee > 7 )
		return;

	int iPosition = (7-iRangee)*8+iColonne;

	// On place la piece.
	if ( i_szCommande[0] == 'x' )
		cb.board[iPosition] = 0;
	else
		cb.board[iPosition] = (Piece)iType;
}

//
// Cette fonction prend un coup en texte et la convertie en un format
// que la machine comprend. Elle comprend les coups en algebrique abbrege.
// 
// Code modifie de Crafty.
//
int Engine::InputMove(char* text, int ply, int wtm, TMove& move) 
{
	int moves[220];
	TMove goodmove;
	int piece=-1, capture, promote, give_check;
	int ffile, frank, tfile, trank;
	int current, i, nleft;
	char *goodchar;
	char movetext[128];
	static const char pieces[17]=
	{' ', ' ', 'P','p','N','n','B','B','R','r','Q','q','K','k','\0'};
	static const char pro_pieces[17]=
	{' ', ' ', 'P','p','N','n','B','b','R','r','Q','q','K','k','\0'};

	/*
	   check for fully-qualified input (f1e1) and handle if needed.
	 */
	if (strlen(text) == 0) return(0);
	if ((text[0] >= 'a') && (text[0] <= 'h') &&
					(text[1] >= '1') && (text[1] <= '8') &&
					(text[2] >= 'a') && (text[2] <= 'h') &&
					(text[3] >= '1') && (text[3] <= '8')) {

			return Engine::Parse(text, ply, wtm, move);
	}

	/*
	   initialize move structure in case an error is found
	 */
	strcpy(movetext,text);
	moves[0]=0;
	piece=0;
	capture=0;
	promote=0;
	give_check=0;
	frank=-1;
	ffile=-1;
	trank=-1;
	tfile=-1;
	goodchar=strchr(movetext,'#');
	if (goodchar) *goodchar=0;
	/*
	   first, figure out what each character means.  the first thing to
	   do is eliminate castling moves
	 */
	if (!strcmp(movetext,"o-o") || !strcmp(movetext,"o-o+") ||
					!strcmp(movetext,"O-O") || !strcmp(movetext,"O-O+") ||
					!strcmp(movetext,"0-0") || !strcmp(movetext,"0-0+")) {
			piece=ROI;
			if(wtm) {
					ffile=4;
					frank=7;
					tfile=6;
					trank=7;
			}
			else {
					ffile=4;
					frank=0;
					tfile=6;
					trank=0;
			}
	}
	else 
			if (!strcmp(movetext,"o-o-o") || !strcmp(movetext,"o-o-o+") ||
							!strcmp(movetext,"O-O-O") || !strcmp(movetext,"O-O-O+") ||
							!strcmp(movetext,"0-0-0") || !strcmp(movetext,"0-0-0+")) {
					piece=ROI;
					if(wtm) {
							ffile=4;
							frank=7;
							tfile=2;
							trank=7;
					}
					else {
							ffile=4;
							frank=0;
							tfile=2;
							trank=0;
					}
			}
			else {
					/*
					   ok, it's not a castling.  check for the first two characters of "bb" which
					   indicates that the first "b" really means "B" since pawn advances don't
					   require a source file.  
					 */
					if ((movetext[0] == 'b') && (movetext[1] == 'b')) movetext[0]='B';
					/*
					   now, start by picking off the check indicator (+) if one is present.
					 */
					if (strchr(movetext,'+')) {
							*strchr(movetext,'+')=0;
							give_check=1;
					}
					/*
					   now, continue by picking off the promotion piece if one is present.  this 
					   is indicated by something like =q on the end of the move string.
					 */
					if (strchr(movetext,'=')) {
							goodchar=strchr(movetext,'=');
							goodchar++;
							promote=(strchr(pro_pieces,*goodchar)-pro_pieces) >> 1;
							*strchr(movetext,'=')=0;
					}
					/*
					   the next thing to do is extract the last rank/file designators since
					   the destination is required.  note that we can have either or both.
					 */
					current=strlen(movetext)-1;
					trank='8'-movetext[current];
					if ((trank >= 0) && (trank <= 7)) 
							movetext[current]=0;
					else 
							trank=-1;
					current=strlen(movetext)-1;
					tfile=movetext[current]-'a';
					if ((tfile >= 0) && (tfile <= 7)) 
							movetext[current]=0;
					else
							tfile=-1;
					if (strlen(movetext)) {
							/*
							   now check the first character to see if it's a piece indicator
							   (PpNnBbRrQqKk).  if so, strip it off.
							 */
							if (strchr("  PpNnBBRrQqKk",*movetext)) {
									piece=(strchr(pieces,movetext[0])-pieces) >> 1;
									for(i=0;i<(int) strlen(movetext);i++) 
											movetext[i]=movetext[i+1];
							}
							/*
							   now that we have the destination and the moving piece (if any)
							   the next step is to see if the last character is now an "x"
							   indicating a capture   if so, set the capture flag, remove the
							   trailing "x" and continue.
							 */
							if ((strlen(movetext)) && (movetext[strlen(movetext)-1] == 'x')) {
									capture=1;
									movetext[strlen(movetext)-1]=0;
							}
							else
									capture=0;
							/*
							   now, all that can be left is a rank designator, a file designator
							   or both.  if the last character a number, then the first (if present)
							   has to be a letter.
							 */
							if (strlen(movetext)) {
									ffile=movetext[0]-'a';
									if ((ffile < 0) || (ffile > 7)) {
											ffile=-1;
											frank='8'-movetext[0];
											if ((frank < 0) || (frank > 7)) piece=-1;
									}
									else {
											if (strlen(movetext) == 2) {
													frank='8'-movetext[1];
													if ((frank < 0) || (frank > 7)) piece=-1;
											}
									}
							}
					}
			}
	if (!piece) piece=1;

/*	if ( piece == 1 && ffile != -1 && !capture ) {

		if ( tfile != ffile ) { 
			capture = 1;
		}
	}*/

	TMoveList movelist;
	GenMoveAttaque(ply, wtm, movelist);
	GenMovePasAttaque(ply, wtm, movelist);

	for (i=0; i<movelist.nbmove; i++ ) {
			if (piece && (movelist.moves[i].Piece != piece)) 
					movelist.moves[i].Piece = 0;
			if ((ffile >= 0) && (File(movelist.moves[i].From)) != ffile) 
					movelist.moves[i].Piece = 0;
			if (capture && (!movelist.moves[i].Capture)) 
					movelist.moves[i].Piece = 0;
			if (promote && (movelist.moves[i].Promotion) != promote) 
					movelist.moves[i].Piece = 0;
			if ((frank >= 0)  && (Rank(movelist.moves[i].From) != frank)) 
					movelist.moves[i].Piece = 0;
			if ((tfile >= 0)  && (File(movelist.moves[i].To) != tfile)) 
					movelist.moves[i].Piece = 0;
			if ((trank >= 0)  && (Rank(movelist.moves[i].To) != trank)) 
					movelist.moves[i].Piece = 0;
			if ( movelist.moves[i].Piece ) {
					MakeMove(ply, movelist.moves[i],  wtm);
					if (Check(wtm) || (give_check && !Check(!wtm))) {
							UnmakeMove(ply, movelist.moves[i], wtm);
							movelist.moves[i].Piece = 0;
					}
					else UnmakeMove(ply, movelist.moves[i], wtm);
			}
	}
	nleft=0;
	for (i=0; i<movelist.nbmove; i++ ) {
			if ( movelist.moves[i].Piece ) {
					nleft++;
					goodmove=movelist.moves[i];
			}
	}
	if (nleft == 1) {
			move = goodmove;
			return 1;
	}
	if (nleft == 0) journal.Log( "Illegal move: %s\n",text);
	else if (piece < 0) journal.Log( "Illegal move (unrecognizable): %s\n", text );
	else journal.Log( "Illegal move (ambiguous): %s\n", text );

	return(0);
}

// Prend un coup sous forme de chaine utilisateur et donne l'equivalent
// sous forme interne.
// Valeur retourne:
//    true si le coup est valide, false sinon.
bool Engine::Parse(const char* i_szEntree, int ply, int wtm, TMove& o_Move)
{
	// Prend l'entree de l'utilisateur et
	// le converti en un format interne.

	// La case source.
	int iRangee = int( i_szEntree[1] )-'0';
	int iColonne = int( i_szEntree[0] )-'a';
	TMove move;
	move.From = (8-iRangee)*8+iColonne;
	if ( move.From > 63 )
		return false;

	// La case destination.
	iRangee = int( i_szEntree[3] )-'0';
	iColonne = int( i_szEntree[2] )-'a';
	move.To = (8-iRangee)*8+iColonne;
	if ( move.To > 63 )
		return false;

	// Vérifier s'il y a une promotion et 
	// si c'est une pièce autre que la dame.
	if (i_szEntree[4] == 'q')
		move.Promotion = dame;
	else if (i_szEntree[4] == 'r')
		move.Promotion = tour;
	else if (i_szEntree[4] == 'b')
		move.Promotion = fou;
	else if (i_szEntree[4] == 'n')
		move.Promotion = cavalier;
	else
		move.Promotion = 0;

	// Maintenant, verifier si le coup est valide.
	// Pour commencer, verifier si la case source a bien une piece et de la bonne
	// couleur.
	if ( wtm )
		if ( cb.board[move.From] <= 0 )
			return false;
		else ;
	else
		if ( cb.board[move.From] >= 0 )
			return false;

	// Verifier si la case destination est vide ou contient une piece adverse.
	Piece piece = cb.board[move.To];
	if ( wtm )
		if ( piece > 0 )
			return false;
		else ;
	else
		if ( piece < 0 )
			return false;

	// Transfere le coup.
	move.Capture = cb.board[move.To];
	if ( move.Capture < 0 )
		move.Capture = -move.Capture;
	move.Piece = cb.board[move.From];
	if ( move.Piece < 0 )
		move.Piece = -move.Piece;
	o_Move = move;
	return true;
}
