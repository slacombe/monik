//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Journal.cpp
// Debut: 2 novembre 1998.
//
//---------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "utile.h"
#include "journal.h"


TJournal::TJournal()
{
  Nouveau();
}

TJournal::~TJournal()
{
}

void TJournal::Nouveau()
{
  // Determiner le dernier fichier de journalisation.
  int iDernierNumeroJournal = TrouverDernierJournal();

  // Fabriquer le nouveau journal.
  sprintf( m_szFichier, "game.%03d", iDernierNumeroJournal+1 );
}

void TJournal::logBoard()
{
	char ligne[100], move[6];
	strcpy(ligne, "");
	for(int i=A8; i<=H1; i++) {
		sprintf(move, "%2d ", cb.board[i]);
		strcat(ligne, move);
		if (i % 8 == 7) {
			Log(ligne);
			strcpy(ligne, "");
		}
	}
}

void TJournal::Log( const char* i_szMessage, ... )
{
  FILE *Fp;
  char szTexte[255];

  Fp = fopen( m_szFichier, "a" );
  if ( !Fp )
    return;

  va_list listeParametres;
  va_start( listeParametres, i_szMessage );
  vsprintf( szTexte, i_szMessage, listeParametres );
  fprintf( Fp, szTexte );
  fprintf( Fp, "\n" );
  va_end( listeParametres );

  fclose( Fp );

  return;
}
