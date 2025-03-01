//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: GenMove.h
// Debut: 4 novembre 1998.
//
//---------------------------------------------------------------------------
#ifndef GenMoveH
#define GenMoveH

#include "board.h"

void genMoveAttaque(TChessBoard *cb, 
                        int ply,                  // Profondeur.
                        int wtm,                 // Joueur
                        LINE& movelist);    // Liste des coups.

void genMovePasAttaque(TChessBoard *cb,
                        int ply,                  // Profondeur.
                        int wtm,                 // Joueur
                        LINE& movelist);    // Liste des coups.

//---------------------------------------------------------------------------
#endif
