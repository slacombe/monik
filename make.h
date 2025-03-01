//-----------------------------------------------------------------------------
// Projet: SLChess
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Make.h
// Debut: 5 novembre 1998.
//
//---------------------------------------------------------------------------
#ifndef MakeH
#define MakeH

void makeMove(TChessBoard *cb, int ply, MOVE move, int wtm);

void makeMoveRoot(TChessBoard *cb, MOVE move, int wtm);
//---------------------------------------------------------------------------
#endif
