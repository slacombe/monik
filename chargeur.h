//---------------------------------------------------------------------------
#ifndef ChargeurH
#define ChargeurH

#include <string>
#include "board.h"

using namespace std;

bool loadPosition(TChessBoard *cb, const string& filename);
bool parseFen(TChessBoard *cb, const string& fen);

//---------------------------------------------------------------------------
#endif
