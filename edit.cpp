//---------------------------------------------------------------------------

#include <string.h>

#include "edit.h"

extern bool g_bEdit;
bool g_bBlanche;

void Edit( TChessBoard& cb, const char* i_szCommande, char* o_szReponse )
{
  // Caractere permis.
  const char* szCode = "PNBRQK.";

  strcpy( o_szReponse, "" );
  // Verifier si changement de couleur.
  if ( strcmp( i_szCommande, "c" ) == 0 ) {
    g_bBlanche = !g_bBlanche;
    return;
  }

  // Enleve toutes les pieces.
  if ( strcmp( i_szCommande, "#" ) == 0 ) {
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

  if ( !g_bBlanche )
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
