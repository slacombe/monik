//---------------------------------------------------------------------------
#ifndef NextH
#define NextH

#include "chess.h"
#include "board.h"

constexpr int FIRST_PHASE = 1;
constexpr int GENERATE_CAPTURE_MOVES = 3;
constexpr int CAPTURE_MOVES = 4;
constexpr int KILLER_MOVE_1 = 5;
constexpr int KILLER_MOVE_2 = 6;
constexpr int NON_CAPTURE_MOVES = 7;
constexpr int GENERATE_NON_CAPTURE_MOVES = 8;
constexpr int HASH_MOVE = 9;
constexpr int NO_MORE_MOVES = 10;

extern int Phase[MAXPLY];

int nextMove(TChessBoard* cb, int ply, int wtm);

//---------------------------------------------------------------------------
#endif
