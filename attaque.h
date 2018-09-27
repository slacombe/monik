//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Attaque.h
// Debut: 10 novembre 1998.
//
//---------------------------------------------------------------------------
#ifndef AttaqueH
#define AttaqueH

#include "board.h"
#include "chess.h"

int Attacked(int i_iCase, int i_bWtm );
Bitboard AttaqueDe(int i_iCase, int wtm );

//---------------------------------------------------------------------------
#endif
