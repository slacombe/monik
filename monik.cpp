//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) March 25, 1998. All right reserved.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Monik.cpp
// Debut: 2 novembre 1998.
//
//
// version date         description
// 0.0     2 nov 1998   Debut du projet.
//
//         4 nov 1998   Je commence a implanter les bitboards.
//
//         5 nov 1998   Implanter la generation des coups des pions.
//                      Implanter la generation des coups des cavaliers.
//                      Implanter MakeMove et UnmakeMove.
//
//         7 nov 1998   Commence l'implantation des coups qui glissent.
//                      Implante la generation des coups horizontaux.
//                      Ouf, implante les coups verticaux. C'etait pas facile.
//                      J'ai pas hate de voir les coups diagonaux.
//
//         8 nov 1998   Ouf, finalement, les mouvements diagonaux fonctionnent.
//                      Un vrai casse-tete.
//                      Ameliore le code de l'interface utilisateur.
//                      Implante presque tout les coups d'attaques.
//
//        10 nov 1998   Enfin, tous les coups possibles sont implantes dans
//                      GenMove et MakeMove. Il va rester a les implanter
//                      dans UnmakeMove, mais avant je vais implanter Attacked.
//                      Attacked est implante.
//
//  0.1   12 nov 1998   Bon. Elle peut jouer une partie complete. Elle a encore
//                      aucune fonction d'evaluation. Je ne vois plus de bug.
//
//  1.0   14 nov 1998   Je voyais plus de bugs, mais y en avait une tonne.
//                      Ca tout sortie lorsque elle c'est mise a faire un
//                      paquet de MakeMove et UnmakeMove.
//                      Ok, premiere version avec l'algorithme negamax et
//                      coupe AlphaBeta. Seulement le materiel est calcule.
//
//  1.1   14 nov 1998   Implante le Iterative Deepening et le Thinking.
//
//  1.2   15 nov 1998   Implante quelque table de positionnement.
//
//  1.3   16 nov 1998   Implante le quiescence. Seulement pour les captures
//                      par exemple. Ca l'a ameliore sa cote enormement.
//
//  1.4   19 nov 1998   Premiers essais avec le killer move.
//
//  1.5    7 jan 1999   Ameliore les heuristiques pour les finales. Ex. Mat avec
//                      un roi et une tour ou une dame, ect.
//
//  1.6   17 jan 1999   J'ai introduit la fonction NextMove pour simplifier
//                      le code dans l'alpha-beta. NextMove retourne le prochain
//                      coup a essayer a chaque niveau de l'arbre.
//                      NextMove commence par le PV move, les captures,
//                      killer move 1, killer move 2 et le reste ensuite.
//                      Elle commence enfin a atteindre des profondeurs
//                      acceptables. Repare aussi quelques que bugs sur
//                      l'edition du board.
//
// 1.7    31 jan 1999   Enfin, elle detecte les nulles par repetition.
//                      J'ai rajoute un cle de hachage et je m'en sers pour
//                      la detection. Elle servira bientot au table
//                      de hachage. (Table de transposition, ect.)
//
// 1.8     8 fev 1999   Elle reconnait, maintenant, les pions passes.
//                      Elle permet des extensions de l'arbre pour un pion
//                      passe. J'ai rajoute aussi de la conaissance pour
//                      les tours.
//
// 1.9    15 mars 1999  J'ai effectue une grosse modification sur sa structure.
//                      Je considerais le premier coup comme le ply 0 et devait
//                      etre le ply 1. J'ai separe la fonction qui itere
//                      de la racine de l'arbre pour ameliore la lisibilite
//                      du code et facilite l'implantation du PVS (Principle
//                      Variation Search). Le pvs se base sur l'idee que tous
//                      les coups cherches qui ne sont pas dans la meilleur
//                      suite de coups retenus par l'ordinateur devraient etre
//                      moins bon que celles ci. On effectue alors une recherche
//                      avec une fenetre minimale n et n+1. On prouve que le
//                      coup est moins bon en verifiant un echec sur la borne
//                      inferieur, sinon, on doit refaire la recherche avec une
//                      fenetre normale. Les gains en vitesse sont
//                      considerables.
//
// 2.2					J'ai corrigé le bug qui faisait qu'elle mettait l'autre pat
//						plutôt que mat faisant une nulle.
//						J'ai aussi corrigé l'analyse.
//						J'ai changé le prompt par Blanc (1) évitant le bug de Winboard
//						qui prenait White (1) pour une indication que blanc avait un mat.
//
// 2.2.1				J'ai finalement trouvé le bug qui faisait qu'elle refusait un coup
//						valide des fois mais très rarement.
//
// 2.2.2				Encore un bug fix, crash au démarrage.
//
// 2.2.3				Corrections de deux bugs, coup illégal et donne une nulle quand
//						ce n'est pas le cas.
//
//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "chess.h"
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

const char* nomProgramme = "Monik v2.2.4\n";

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

