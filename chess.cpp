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
