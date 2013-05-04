//---------------------------------------------------------------------------
#ifndef NextH
#define NextH

#include "chess.h"
#include "board.h"

#define  FIRST_PHASE                 1
#define  PV_MOVE                     2
#define  GENERATE_CAPTURE_MOVES      3
#define  CAPTURE_MOVES               4
#define  KILLER_MOVE_1               5
#define  KILLER_MOVE_2               6
#define  NON_CAPTURE_MOVES           7
#define  GENERATE_NON_CAPTURE_MOVES  8
#define  HASH_MOVE                   9
#define  NO_MORE_MOVES              10

extern int Phase[MAXPLY];

int NextMove( TChessBoard& cb, int ply, int wtm );

//---------------------------------------------------------------------------
#endif
