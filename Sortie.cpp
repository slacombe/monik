//---------------------------------------------------------------------------

#include "stdafx.h"

#include <string.h>
#include <stdio.h>

#include "sortie.h"
#include "board.h"
#include "chess.h"
#include "utile.h"

#include "journal.h"

// Variable indiquant si l'interface graphique est utilisee.
extern bool xboard;
extern int iMateInPly;

// Prend un move et genere une chaine lisible.
int SortieMove( TMove& move, char* i_szString )
{
  // La case source.
  i_szString[0] = (char)(move.From % 8 + 'a');
  i_szString[1] = (char)(8-(move.From >> 3)+'0');

  // La case destination.
  i_szString[2] = (char)(move.To % 8 + 'a');
  i_szString[3] = (char)(8-(move.To >> 3)+'0');

  i_szString[4] = 0;

  return true;
}

void Sortie( const char* i_szReponse )
{
  if ( xboard ) {
	printf( "move %s\n", i_szReponse );
  }
  else {
	printf( "\nMy move %s\n", i_szReponse );
  }
}

// Obtient la continuation.
void GetPV( char* o_szContinuation, TMove* ml, int ply )
{
  char szMove[6];
  strcpy( o_szContinuation, "" );
  int iNbMove = pv_length[ply];
  for( int i=0; i<iNbMove; i++ ) {
	  if (!(ml[i].From | ml[i].To))
		  break;
      SortieMove( ml[i], szMove );
	  strcat( o_szContinuation, szMove );
	  strcat( o_szContinuation, " " );
  }
}

void postThinking(int profondeur, int score, int elapsed, int nodes, char* continuation)
{
  printf( "%d %d %d %d %s\n", profondeur, score, elapsed, nodes, continuation);
  fflush(stdout);
}

#ifdef DEBUG
// Obtient la continuation.
void GetCurrent( TChessBoard& cb, char* o_szPath, int deep )
{
  char szMove[6];
  strcpy( o_szPath, "" );
  for( int i=0; i<cb.MoveList[deep].nbmove; i++ ) {
    SortieMove( cb.MoveList[deep].moves[i], szMove );
    strcat( o_szPath, szMove );
    strcat( o_szPath, " " );
  }
}
#endif

void OutBookInfo( char* buffer )
{
	strcpy( buffer, "Book - " );
	for( int i=0; i<pv_length[0]; i++ ) {
		char szMove[10];
		SortieMove( pv[0][i], szMove );
		strcat( buffer, szMove );
		strcat( buffer, ": " );
		char szPercent[10];
		sprintf( szPercent, "%d%%", pv[0][i].Score );
		strcat( buffer, szPercent );
		if ( i < pv_length[0]-1 ) {
			strcat( buffer, ", " );
		}
	}
}

void PrintBoard()
{
	int i;

	for(i=A8; i<=H1; i++) {
		printf(" %2d", cb.board[i]);
		if (i % 8 == 7) 
			printf("\n");
	}
	printf("\n");
}

void PrintBitBoard(Bitboard bitboard)
{
	int i;
	for(i=A8; i<=H1; i++) {
		printf("%d", bitboard & mask[i]);
		if (i % 8 == 7)
			printf("\n");
	}
	printf("\n");
}

