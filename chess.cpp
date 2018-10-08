#include <stdlib.h>

#include "chess.h"
#include "board.h"
#include "make.h"
#include "utile.h"
#include "unmake.h"
#include "log.h"

Bitboard ClePionB[BOARDSIZE];
Bitboard ClePionN[BOARDSIZE];
Bitboard CleCavalierB[BOARDSIZE];
Bitboard CleCavalierN[BOARDSIZE];
Bitboard CleFouB[BOARDSIZE];
Bitboard CleFouN[BOARDSIZE];
Bitboard CleTourB[BOARDSIZE];
Bitboard CleTourN[BOARDSIZE];
Bitboard CleDameB[BOARDSIZE];
Bitboard CleDameN[BOARDSIZE];
Bitboard CleRoiB[BOARDSIZE];
Bitboard CleRoiN[BOARDSIZE];

char MonikHome[255];
unsigned char dernier_bits[65536];
bool g_bAbort = false;
bool g_bModeAnalyse = false;


int ValeurPiece[] = { 0, VALPION, VALCAVALIER, VALFOU,
                         VALTOUR, VALDAME, VALROI };


// Enleve tout ce qui est en echec.
void TMoveList::ChoisiMove(int ply, int wtm)
{
#ifdef JOURNAL
	log.log( "ChoisiMove: NbMove: %d\n", nbmove );
#endif
  if ( nbmove == 0 )
    return;

  currmove = 0;
  int iDernierValide = -1;
  // Verifier chaque coup.
  while( currmove < nbmove ) {

    // Effectuer le coup et verifier si est en echec.
    MakeMove(ply, moves[currmove], wtm);
    int bEstEchec = Check(wtm);
    UnmakeMove(ply, moves[currmove], wtm);

    // Si le coup est valide le copier dans la case suivant le dernier
    // coup valide trouver.
    if ( !bEstEchec ) {
      iDernierValide++;
      moves[iDernierValide] = moves[currmove];
    }

    currmove++;
  }
  // Nombre de coup valide.
  nbmove = iDernierValide+1;
}

// Enleve tout ce qui est en echec.
void TMoveList::ChoisiNonQuiet(int ply, int wtm)
{
  // Verifier chaque coup.
  for( int i=0; i<nbmove; i++ ) {

    moves[i].Score =
      ValeurPiece[moves[i].Capture] - ValeurPiece[moves[i].Piece];
  }
}
