//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Chargeur.cpp
// Debut: 18 janvier 1999.
//
//---------------------------------------------------------------------------
#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "chess.h"
#include "board.h"

#include "chargeur.h"

extern int wtm;

bool ChargePosition( TChessBoard& cb, const char* i_szFichier )
{
  // Verifier si le fichier existe.
  // Si il n'existe pas retourner une erreur.
  FILE *fp;
  fp = fopen( i_szFichier, "rb" );
  if ( !fp ) return false;

  // Vider l'echiquier.
  memset( &cb, 0, sizeof( cb ) );

  // La position FEN ce lit comme suit.
  // Commencer a la case A8. Lire chaque caractere.
  // Les caracteres signifient ceci:
  // #   Un chiffre de 1 a 7. Donne le nombre de case vide avant
  //     la prochaine piece.
  // x   Une piece noir pour les minuscules ou une piece blanche pour
  //     les majuscules.
  //     P = pion, N = cavalier, B = fou, R = tour, Q = dame, K = king.
  // /   Saute a la prochaine rangee.

  int iPosCourante = A8;
  int ch = 0;
  while( ch != ' ' && !feof( fp ) ) {
    // Lecture d'un caractere.
    ch = fgetc( fp );

    // Verifier si c'est un chiffre.
    if ( isdigit( ch ) ) {
      // Augmenter du nombre de case specifie.
      iPosCourante += ch-'0';
    }
    else if ( isalpha( ch ) ) {
      int bWhite = isupper( ch );
      ch = toupper( ch );
      const char* szCode = "PNBRQK";
      const char* pPiece = strchr( szCode, ch );
      if ( !pPiece ) {
        fclose( fp );
        return false;
      }
      int iPiece = pPiece - szCode + 1;
      if ( !bWhite ) {
         iPiece = -iPiece;
      }
      cb.board[iPosCourante++] = (Piece)iPiece;
    }
    else if ( ch == '/' ) {
      iPosCourante = ((iPosCourante+7)/8)*8;
    }
  } // while.

  // C'est a qui a jouer.
  ch = fgetc( fp );
  if ( ch == 'w' )
    wtm = true;
  else if ( ch == 'b' )
    wtm = false;
  else
    return false;

  fclose( fp );
  cb.InitialiseBitboard();
  
  return true;
}
