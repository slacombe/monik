//---------------------------------------------------------------------------
#ifndef IterationH
#define IterationH

#include "board.h"
#include "chess.h"

extern int maxply;
extern int prev_root_score, root_score, root_alpha, root_beta;
extern int timestamp, timeslot, timeabort, timeextension;
extern int nbevals, iNodes;

int Iteration(int wtm);

//---------------------------------------------------------------------------
#endif
