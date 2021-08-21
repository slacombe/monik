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
#include "board.h"
#include "logger/log.h"
#include "init.h"
#include "transposition.h"
#include "test.h"
#include "controller/ControllerFactory.h"
#include "controller/Controller.h"

//---------------------------------------------------------------------------
extern int interrupted;
extern int wtm;

const char* nomProgramme = "Monik v2.2.7";
Controller* controller;

void Interrupt(int)
{
	if (controller) {
		controller->interruptReceived();
	}
}

int main(int argc, char **argv)
{
	ControllerFactory* controllerFactory = new ControllerFactory();
	controller = controllerFactory->createController();
		
	signal( SIGINT, Interrupt );
	signal( SIGTERM, Interrupt );	

  // Initialisation.
  TMoveList movelist;

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
	  
	  if (strcmp(argv[i], "-gamelog") == 0) {
		  gameLog.turnOn();
	  }
    }
  }

#ifdef TRANSPOSITION
  // Verifier si une table de transposition a ete construite.
  if ( !TableTrans )
    // Au moins 1 meg de transposition.
    TableTrans = new TTableTrans( 32 );
#endif
  
  controller->run();

#ifdef TRANSPOSITION
  if ( TableTrans )
    delete TableTrans;
#endif

  return 0;
}

