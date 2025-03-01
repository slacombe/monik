//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Attaque.cpp
// Debut: 10 novembre 1998.
//
//---------------------------------------------------------------------------
#ifndef UnmakeH
#define UnmakeH

#include "chess.h"
#include "board.h"

void unmakeMove(TChessBoard *cb, int ply, MOVE i_Move, int wtm);
void undoMove(TChessBoard *cb);

//---------------------------------------------------------------------------
#endif

