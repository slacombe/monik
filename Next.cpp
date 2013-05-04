//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Monik.cpp
// Debut: 10 janvier 1998.
//
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <stdio.h>
#include "genmove.h"
#include "valide.h"
#include "next.h"
#include "utile.h"

int Phase[MAXPLY];

// Cette fonction genere les coups pour chaque phase
int NextMove( TChessBoard& cb, int ply, int wtm )
{
  register int i, max, maxpos;
  TMove temp;

  switch( Phase[ply] ) {
    case FIRST_PHASE:
#ifdef TRANSPOSITION
      Phase[ply] = HASH_MOVE;
#else
      Phase[ply] = PV_MOVE;
#endif
      return false;
    case HASH_MOVE:
      Phase[ply] = PV_MOVE;
      if (ValideMove(ply, wtm, cb.HashMove[ply])) {
        cb.MoveList[ply].currmove = 0;
        cb.MoveList[ply].moves[0] = cb.HashMove[ply];
        cb.MoveList[ply].nbmove = 1;
        return true;
      }
      else
        return false;
    case PV_MOVE:
      Phase[ply] = GENERATE_CAPTURE_MOVES;
      if (ValideMove(ply, wtm, pv[1][ply])) {
        cb.MoveList[ply].currmove = 0;
        cb.MoveList[ply].moves[cb.MoveList[ply].currmove] =
          pv[1][ply];
        cb.MoveList[ply].nbmove = 1;
        return true;
      }
      else
       return false;
    case GENERATE_CAPTURE_MOVES:
      Phase[ply] = CAPTURE_MOVES;
      cb.MoveList[ply].currmove = -1;
      cb.MoveList[ply].nbmove = 0;
      GenMoveAttaque(ply, wtm, cb.MoveList[ply]);
      if ( cb.MoveList[ply].nbmove == 0 ) {
        Phase[ply] = KILLER_MOVE_1;
        return false;
      }

      // Calculer le gain estimer pour chacun des coups.
      // La valeur de la piece capture moins la valeur de la piece qui capture.
      for( i=0; i<cb.MoveList[ply].nbmove; i++ ) {
        int iScoreGain =
          ValeurPiece[cb.MoveList[ply].moves[i].Capture]-
          ValeurPiece[cb.MoveList[ply].moves[i].Piece];
/*        if ( iScoreGain < 0 ) {
          iScoreGain = Echange( cb,
                                cb.MoveList[ply].moves[i].From,
                                cb.MoveList[ply].moves[i].To,
                                wtm );

        }*/
        cb.MoveList[ply].moves[i].Score = iScoreGain;
      }
      //return false;

    case CAPTURE_MOVES:
      // Prendre le meilleur coup de la liste et le mettre au debut.
      cb.MoveList[ply].currmove++;
      if ( cb.MoveList[ply].nbmove == 0 ) {
        Phase[ply] = KILLER_MOVE_1;
        return false;
      }
      if ( cb.MoveList[ply].currmove >= cb.MoveList[ply].nbmove ) {
        Phase[ply] = KILLER_MOVE_1;
        return false;
      }
      max = -INFINI; maxpos = -1;
      for( i=cb.MoveList[ply].currmove; i<cb.MoveList[ply].nbmove; i++ ) {
        if ( cb.MoveList[ply].moves[i].Score > max ) {
          max = cb.MoveList[ply].moves[i].Score;
          maxpos = i;
        }
      }
      if ( max < 0 || maxpos == -1 ) {
        Phase[ply] = KILLER_MOVE_1;
        return false;
      }
      if ( maxpos == cb.MoveList[ply].currmove ) {
        return true;
      }
      temp = cb.MoveList[ply].moves[maxpos];
      cb.MoveList[ply].moves[maxpos] =
        cb.MoveList[ply].moves[cb.MoveList[ply].currmove];
      cb.MoveList[ply].moves[cb.MoveList[ply].currmove] = temp;
      return true;
    case KILLER_MOVE_1:
      Phase[ply] = KILLER_MOVE_2;
      if (ValideMove(ply, wtm, cb.Killers[ply][0])) {
        cb.MoveList[ply].moves[0] = cb.Killers[ply][0];
        cb.MoveList[ply].currmove = 0;
        cb.MoveList[ply].nbmove = 0;
        return true;
      }
      return false;
    case KILLER_MOVE_2:
      Phase[ply] = GENERATE_NON_CAPTURE_MOVES;
      if (ValideMove(ply, wtm, cb.Killers[ply][1])) {
        cb.MoveList[ply].moves[0] = cb.Killers[ply][1];
        cb.MoveList[ply].currmove = 0;
        cb.MoveList[ply].nbmove = 0;
        return true;
      }
      return false;
    case GENERATE_NON_CAPTURE_MOVES:
      Phase[ply] = NON_CAPTURE_MOVES;
      cb.MoveList[ply].nbmove = 0;
      GenMoveAttaque(ply, wtm, cb.MoveList[ply]);
      GenMovePasAttaque(ply, wtm, cb.MoveList[ply]);
      cb.MoveList[ply].currmove = -1;
      if ( cb.MoveList[ply].nbmove == 0 ) {
        Phase[ply] = NO_MORE_MOVES;
        return false;
      }
//      return false;
    case NON_CAPTURE_MOVES:
      if ( cb.MoveList[ply].nbmove == 0 ) {
        Phase[ply] = NO_MORE_MOVES;
        return false;
      }

      // Trouver le prochain coup qui n'est pas une capture ou
      // que la valeur de la piece capturee moins celle qui capture
      // est negative.
      int iScoreCapture;
      while( cb.MoveList[ply].currmove < cb.MoveList[ply].nbmove-1 ) {
        cb.MoveList[ply].currmove++;
        if ( cb.MoveList[ply].CurrentMove().Capture == 0 )
          return true;
        iScoreCapture = ValeurPiece[cb.MoveList[ply].CurrentMove().Capture] -
                        ValeurPiece[cb.MoveList[ply].CurrentMove().Piece];
        if ( iScoreCapture <= 0 )
          return true;
      }
      Phase[ply] = NO_MORE_MOVES;
      return false;
  }


  return false;
}
