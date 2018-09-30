//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) March 25, 1998. All right reserved.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Monik.cpp
// Debut: 2 novembre 1998.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "chess.h"
#include "book.h"
#include "board.h"
#include "entree.h"
#include "sortie.h"
#include "engine.h"
#include "journal.h"
#include "init.h"
#include "utile.h"
#include "transposition.h"
#include "test.h"

//---------------------------------------------------------------------------
TJournal journal;
extern int interrupted;
extern int wtm;

//---------------------------------------------------------------------------

const char* nomProgramme = "Monik v2.2.6\n";

void Interrupt( int )
{
	interrupted = true;
}

int main(int argc, char **argv)
{
  // Initialisation.
  TMoveList movelist;
  setbuf(stdout, 0);
  signal( SIGINT, Interrupt );
  signal( SIGTERM, Interrupt );
  printf( "%s", nomProgramme );
  printf( "Copyright(C) 2009-2011.\n" );
  printf( "Sylvain Lacombe\n\n" );

  char* temphome = getenv("MONIKHOME");
  if (temphome) {
    strcpy(MonikHome, temphome);
  }

#ifdef TRANSPOSITION
  TableTrans = 0;
#endif

  // Maintenant, verifier si il y a des parametres.
  if ( argc > 1 ) {
    // Survoler la liste des parametres et les traiter si besoin.
    for( int i=1; i<argc; i++ ) {
#ifdef  TRANSPOSITION
      // Verifier pour le parametre des tables de transpositions.
      if ( strcmp( argv[i], "-h" ) == 0 ) {
        if ( argc < i+2 || atoi( argv[i+1] ) == 0 ) {
          printf( "\nMauvaise largeur de table." );
          return 1;
        }
        TableTrans = new TTableTrans( atoi( argv[i+1] ) );
        TableTrans->Initialise();
      }

	  if (strcmp(argv[i], "test") == 0) {
		  testTransposition();
		  return 0;
	  }

#endif // TRANSPOSITION
    }
  }

#ifdef TRANSPOSITION
  // Verifier si une table de transposition a ete construite.
  if ( !TableTrans )
    // Au moins 1 meg de transposition.
    TableTrans = new TTableTrans( 32 );
#endif
  
  openingBook->load();

  // On fonctionne ainsi.
  // On boucle entre l'entree de commande et
  // la reponse de l'ordinateur.
  char szCommande[255];
  char szReponse[255];
  TSeeker::start();
  do {

    g_bAbort = false;
	fflush( 0 );
   	Entree( szCommande );
    
    bool bSucces = Engine( szCommande, szReponse );

    if ( !bSucces ) {
      // Soit un coup invalide ou la reponse a une option.
      printf( "%s\n", szReponse );
	  fflush( stdout );
    }
    else {
      // On retourne
      if ( !g_bModeAnalyse ) {
        Sortie( szReponse );
      }
    }
#ifdef TRANSPOSITION
      // Cette fonction peut etre longue a etre executer.
      // C'est pourquoi je l'ai mise apres avoir envoye le coup a WinBoard.
      TableTrans->Initialise();
#endif
  } while( strcmp( szCommande, "quit" ) );

#ifdef TRANSPOSITION
  if ( TableTrans )
    delete TableTrans;
#endif

  return 0;
}

