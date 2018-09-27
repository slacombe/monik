//---------------------------------------------------------------------------
#ifndef RacineH
#define RacineH

#include "board.h"

extern int maxply;
int SearchRacine(int depth, int wtm, int alpha, int beta);
void AffichePV(int i_iIteration);

//---------------------------------------------------------------------------
#endif
