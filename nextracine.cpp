//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Monik.cpp
// Debut: 25 janvier 1998.
//
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <stdio.h>

#pragma hdrstop

#include "genmove.h"
#include "valide.h"
#include "next.h"
#include "utile.h"

// Cette fonction genere les coups pour chaque phase
int NextMoveRacine( TChessBoard& cb, int wtm )
{
  cb.MoveList[1].currmove++;
  if ( cb.MoveList[1].nbmove == 0 ) {
    Phase[1] = NO_MORE_MOVES;
    return false;
  }

  if ( cb.MoveList[1].currmove >= cb.MoveList[1].nbmove ) {
    Phase[1] = NO_MORE_MOVES;
//    cb.MoveList[1].currmove--;
    return false;
  }
  
  return true;
}
 
