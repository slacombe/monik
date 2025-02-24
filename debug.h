//---------------------------------------------------------------------------
#ifndef DebugH
#define DebugH

#include <string>

#include "chess.h"
#include "board.h"

#ifdef DEBUG
void consistence(TChessBoard *cb, int wtm, const char* caller, TMove move);
#endif

//---------------------------------------------------------------------------
#endif
