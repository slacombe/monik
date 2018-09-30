//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Entree.cpp
// Debut: 5 novembre 1998.
//
//---------------------------------------------------------------------------
#ifndef EntreeH
#define EntreeH

#include "chess.h"
#include "board.h"

void Entree( char* o_szCommande );

bool Parse(const char* i_szEntree, int ply, int wtm, TMove& o_Move);

bool Option( const char* i_szCommande, char* o_szReponse );

int InputMove(const char* text, int ply, int wtm, TMove& move);
//---------------------------------------------------------------------------
#endif
