#ifndef EntreeH
#define EntreeH

#include <string>
#include "chess.h"
#include "board.h"

using namespace std;

extern bool exiting;

void entree(TChessBoard *cb, char* o_szCommande);

bool parse(TChessBoard *cb, const char* i_szEntree, int ply, int wtm, TMove& o_Move);

bool option(TChessBoard *cb, const string command, string& response);

int inputMove(TChessBoard *cb, char* text, int ply, int wtm, TMove& move);
//---------------------------------------------------------------------------
#endif
