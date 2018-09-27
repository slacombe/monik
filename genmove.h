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

void GenMoveAttaque( int ply,                  // Profondeur.
                     int wtm,                 // Joueur
                     TMoveList& movelist );    // Liste des coups.

void GenMovePasAttaque(int ply,                  // Profondeur.
                       int wtm,                 // Joueur
                       TMoveList& movelist );    // Liste des coups.

//---------------------------------------------------------------------------
#endif
