//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: ValideMove.cpp
// Debut: 26 novembre 1998.
//
//
//---------------------------------------------------------------------------

#include <stdlib.h>

#include "valide.h"
#include "board.h"
#include "attaque.h"
#include "utile.h"

// Pour voir si le killer move est toujours valide avant de le jouer.
bool ValideMove(int ply, int wtm, TMove& move)
{
  // Verifier si la case de depart contient la bonne piece.
  if ( move.Piece < 1 || move.Piece > 6 )
    return false;
  if ( move.Capture < 0 || move.Capture > 5 )
    return false;


  if ( wtm ) {
    if ( cb.board[move.From] != move.Piece )
      return false;
  }
  else {
    if ( cb.board[move.From] != -move.Piece )
      return false;
  }

  // Verifier les coups un peu special.
  switch( move.Piece ) {
    case 0:
      return false;
    case roi:
      // Verifier le roque du roi seulement.
      if ( abs( int(move.To - move.From) ) == 2 ) {
        if ( wtm ) {
          // Roque du roi.
          if ( move.To == G1 ) {
            if ( !(cb.Roque & ROQUEROIBLANC) &&
                 (cb.vide & 0x6000000000000000LL) == 0x6000000000000000LL &&
                 !Attacked(E1, !wtm) &&
                 !Attacked(F1, !wtm) &&
                 !Attacked(G1, !wtm) )
              return true;
            else
              return false;
          }
          else {
            // Roque cote dame.
            if ( !(cb.Roque & ROQUEDAMEBLANC) &&
                 (cb.vide & 0x0E00000000000000LL) == 0x0E00000000000000LL &&
                 !Attacked(E1, !wtm) &&
                 !Attacked(D1, !wtm) &&
                 !Attacked(C1, !wtm) )
              return true;
            else
              return false;
          }
        }
        else {
          if ( move.To == G8 ) {
            // Roque cote roi.
            if ( !(cb.Roque & ROQUEROINOIR) &&
                 (cb.vide & 0x60 ) == 0x60 &&
                 !Attacked(E8, !wtm) &&
                 !Attacked(F8, !wtm) &&
                 !Attacked(G8, !wtm) )
              return true;
            else
              return false;
          }
          else {
            if ( !(cb.Roque & ROQUEDAMENOIR) &&
                 (cb.vide & 0x0E) == 0x0E &&
                 !Attacked(E8, !wtm) &&
                 !Attacked(D8, !wtm) &&
                 !Attacked(C8, !wtm))
              return true;
            else
              return false;
          }
        }
      }
      break;
    case pion:
        // Verifier le pion qui avance et le enpassant. La capture est
        // verifier avec les autre pieces normales.
        // Le pion qui avance d'une case.
        if ( wtm ) {
          if ( move.To - move.From == -8 ) {
            if ( move.Capture == 0 && cb.board[move.To] == 0 )
              return true;
            else
              return false;
          }
          else if ( move.To - move.From == -16 ) {
            if ( move.Capture == 0 && cb.board[move.To+8] == 0 &&
                 cb.board[move.To] == 0 )
              return true;
            else
              return false;
          }
        }
        else {
          if ( move.To - move.From == 8 ) {
            if ( move.Capture == 0 && cb.board[move.To] == 0 )
              return true;
            else
              return false;
          }
          else if ( abs( int(move.To - move.From) ) == 16 ) {
            if ( move.Capture == 0 && cb.board[move.To-8] == 0 &&
                 cb.board[move.To] == 0 )
              return true;
            else
              return false;
          }
        }
        
        if ( move.EnPassant ) {
          return false;
          if ( wtm ) {
            if ( (unsigned)cb.EnPassantN[ply] == move.To &&
                 move.Capture == pion )
              return true;
            else
              return false;
          }
          else {
            if ( (unsigned)cb.EnPassantB[ply] == move.To &&
                 move.Capture == pion )
              return true;
            else
              return false;
          }
        }
      return false;
  }

  if ( wtm ) {
    if ( -move.Capture == cb.board[move.To] &&
         (AttaqueDe(move.To, wtm) & mask[move.From]) )
      return true;
  }
  else {
    if (move.Capture == cb.board[move.To] &&
         (AttaqueDe(move.To, wtm ) & mask[move.From]))
      return true;
  }

  return false;
}
