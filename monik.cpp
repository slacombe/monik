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
#include "entree.h"
#include "sortie.h"
#include "engine.h"
#include "log.h"
#include "init.h"
#include "utile.h"
#include "transposition.h"
#include "test.h"

//---------------------------------------------------------------------------
extern int interrupted;
extern int wtm;

//---------------------------------------------------------------------------

const char *nomProgramme = "Monik v2.2.8";

void Interrupt(int)
{
  interrupted = true;
}

int main(int argc, char **argv)
{
  // Initialisation.
  TMoveList movelist;
  setbuf(stdout, 0);
  signal(SIGINT, Interrupt);
  signal(SIGTERM, Interrupt);
  printf("%s\n", nomProgramme);
  printf("Copyright(C) 2009-2011.\n");
  printf("Sylvain Lacombe\n\n");

  char *temphome = getenv("MONIKHOME");
  if (temphome)
  {
    strcpy(MonikHome, temphome);
  }

  // Maintenant, verifier si il y a des parametres.
  if (argc > 1)
  {
    // Survoler la liste des parametres et les traiter si besoin.
    for (int i = 1; i < argc; i++)
    {
#ifdef TRANSPOSITION
      // Verifier pour le parametre des tables de transpositions.
      if (strcmp(argv[i], "-h") == 0)
      {
        if (argc < i + 2 || atoi(argv[i + 1]) == 0)
        {
          printf("\nMauvaise largeur de table.");
          return 1;
        }
        int tableSize = atoi(argv[i + 1]);
        createTranspositionTable(tableSize);
        initializeTranspositionTable();
      }

#endif // TRANSPOSITION

      if (strcmp(argv[i], "-gamelog") == 0)
      {
        gameLog.turnOn();
      }
    }
  }

#ifdef TRANSPOSITION
  // Verifier si une table de transposition a ete construite.
  if (!transpositionTableCreated())
  {
    // At least 32 meg of transposition table.
    createTranspositionTable(32);
    initializeTranspositionTable();
  }
#endif

  // On fonctionne ainsi.
  // On boucle entre l'entree de commande et
  // la reponse de l'ordinateur.
  char szCommande[255];
  char szReponse[255];
  TSeeker::start();
  do
  {

    g_bAbort = false;
    fflush(0);
    Entree(szCommande);

    bool bSucces = Engine(szCommande, szReponse);

    if (!bSucces)
    {
      // Soit un coup invalide ou la reponse a une option.
      printf("%s\n", szReponse);
      fflush(stdout);
    }
    else
    {
      // On retourne
      if (!g_bModeAnalyse)
      {
        Sortie(szReponse);
      }
    }
#ifdef TRANSPOSITION
    // Cette fonction peut etre longue a etre executer.
    // C'est pourquoi je l'ai mise apres avoir envoye le coup a WinBoard.
    initializeTranspositionTable();
#endif
  } while (strcmp(szCommande, "quit"));

#ifdef TRANSPOSITION
  if (transpositionTableCreated())
  {
    freeTranspositionTable();
  }
#endif

  return 0;
}
