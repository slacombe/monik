//---------------------------------------------------------------------------
#ifndef RacineH
#define RacineH

#include "board.h"

extern int maxply;
int searchRacine(TChessBoard *cb, int depth, int wtm, int alpha, int beta);
void affichePV(TChessBoard *cb, int i_iIteration);

//---------------------------------------------------------------------------
#endif
