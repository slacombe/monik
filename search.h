//---------------------------------------------------------------------------
#ifndef SearchH
#define SearchH

#include "board.h"

extern int maxply;

int search(TChessBoard *cb, int depth, int ply, int wtm, int alpha, int beta, bool do_null);

#endif
