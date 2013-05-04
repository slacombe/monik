//---------------------------------------------------------------------------
#ifndef SearchH
#define SearchH

#include "board.h"

extern int maxply;
extern int pvsresearch;

int Search(int depth, int ply, int wtm, int alpha, int beta, bool do_null);

#endif
