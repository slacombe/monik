#ifndef EntreeH
#define EntreeH

#include "chess.h"
#include "board.h"

void entree(TChessBoard *cb, char* o_szCommande);

bool parse(TChessBoard *cb, const char* i_szEntree, int ply, int wtm, TMove& o_Move);

bool option(TChessBoard *cb, const char* i_szCommande, char* o_szReponse);

int inputMove(TChessBoard *cb, char* text, int ply, int wtm, TMove& move);
//---------------------------------------------------------------------------
#endif
