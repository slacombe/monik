//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: GenMove.cpp
// Debut: 4 novembre 1998.
//
//---------------------------------------------------------------------------

#include <string.h>

#include "board.h"
#include "genmove.h"
#include "utile.h"
#include "attaque.h"


//-----------------------------------------------------------------------------
// Genere tout les moves possibles qui attaque.
//
void GenMoveAttaque(int ply,                  // Profondeur.
                    int wtm,                 // Joueur
                    TMoveList& movelist )     // Liste des coups.
{
  Bitboard piecebd, temp;
  TMove move;
  move.Score = 0;
  move.Roque = 0;

  // Les blancs.
  if ( wtm ) {

    // Les cavaliers blancs.
    move.EnPassant = 0;
    move.Promotion = 0;
    piecebd = cb.cavalierb;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (cb.AttaqueCavalier[move.From] & cb.piecen);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = cavalier;
        move.Capture = -cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les tours blanches.
    piecebd = cb.tourb;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueTour(move.From)&cb.piecen);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = tour;
        move.Capture = -cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les dames blanches.
    piecebd = cb.dameb;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueDame(move.From)&cb.piecen);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = dame;
        move.Capture = -cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les fous blancs.
    piecebd = cb.foub;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueFou(move.From)&cb.piecen);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = fou;
        move.Capture = -cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    //------------------------------------------------------------
    // Les pions blancs.
    // Les promotions.
    Bitboard pavance1 = (cb.pionb >> 8);
    pavance1 &= cb.vide;
    pavance1 &= 0xFF00000000000000LL;

    // On prend chaque bit et on genere sont move.
    while( pavance1 ) {
      // Insertion du move.
      move.To = DernierBit( pavance1 );
      move.From = move.To+8;
      move.Piece = pion;
      movelist.Ajoute( move );
      // On enleve le bit.
      ClearBit( pavance1, move.To );
    }
    // Les pions blancs.
    Bitboard pprise;
    // On ajoute le pion pouvant etre pris en passant
    // si c'est le cas.
    Bitboard temppiecen = cb.piecen;
    if ( cb.EnPassantN[ ply ] >= 0 )
      temppiecen |= mask[cb.EnPassantN[ ply ]];
    // Prise par la droite.
    // On enleve la colonne H.
    piecebd = cb.pionb & 0xFEFEFEFEFEFEFEFELL;
    piecebd >>= 9;
    pprise = piecebd & temppiecen;
    // Prise par la gauche.
    piecebd = cb.pionb & 0x7F7F7F7F7F7F7F7FLL;
    piecebd >>= 7;
    pprise |= piecebd & temppiecen;
    while( pprise ) {
      move.To = DernierBit( pprise );
      move.Piece = pion;
      move.Capture = -cb.board[move.To];
      if ( move.To % 8 != 0 && cb.board[move.To+7] == pion ) {
        move.From = move.To + 7;
        if ( move.To < 8 ) {
          // Une promotion.
          move.Promotion = dame;
          movelist.Ajoute( move );
          move.Promotion = tour;
          movelist.Ajoute( move );
          move.Promotion = fou;
          movelist.Ajoute( move );
          move.Promotion = cavalier;
          movelist.Ajoute( move );
        }
        else {
          move.Promotion = 0;
          if ( cb.board[move.To] != 0 ) {
            move.EnPassant = 0;
          }
          else {
            // En passant.
            move.Capture = pion;
            move.EnPassant = 1;
          }
          movelist.Ajoute( move );
        }
      }
      if ( move.To % 8 != 7 && cb.board[move.To+9] == pion ) {
        move.From = move.To + 9;
        if ( move.To < 8 ) {
          // Une promotion.
          move.Promotion = dame;
          movelist.Ajoute( move );
          move.Promotion = tour;
          movelist.Ajoute( move );
          move.Promotion = fou;
          movelist.Ajoute( move );
          move.Promotion = cavalier;
          movelist.Ajoute( move );
        }
        else {
          if ( cb.board[move.To] != 0 ) {
            move.EnPassant = 0;
          }
          else {
            // En passant.
            move.Capture = pion;
            move.EnPassant = 1;
          }
          move.Promotion = 0;
          movelist.Ajoute( move );
        }
      }
      ClearBit( pprise, move.To );
    }

    // Le roi blancs.
    piecebd = cb.roib;
    move.EnPassant = 0;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (cb.AttaqueRoi[move.From] & cb.piecen);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = roi;
        move.Capture = -cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }
  }
//----------------------------------------------------------------------------
//
// Les pieces noirs.
//
//
//----------------------------------------------------------------------------
  else {     // Les noirs.

    // Les cavaliers noirs.
    piecebd = cb.cavaliern;
    move.EnPassant = 0;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (cb.AttaqueCavalier[move.From]) & cb.pieceb;
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = cavalier;
        move.Capture = cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les dames noires.
    piecebd = cb.damen;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueDame(move.From)&cb.pieceb);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = dame;
        move.Capture = cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les tours noirs.
    piecebd = cb.tourn;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueTour(move.From)&cb.pieceb);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = tour;
        move.Capture = cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les fous noirs.
    piecebd = cb.foun;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueFou(move.From)&cb.pieceb);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = fou;
        move.Capture = cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    //------------------------------------------------------------
    // Les promotions.
    Bitboard pavance1 = (cb.pionn << 8);
    pavance1 &= cb.vide;
    pavance1 &= 0x00000000000000FF;
    
    // On prend chaque bit et on genere sont move.
    while( pavance1 ) {
      // Insertion du move.
      move.To = DernierBit( pavance1 );
      move.From = move.To-8;
      move.Piece = pion;
      movelist.Ajoute( move );
      // On enleve le bit.
      ClearBit( pavance1, move.To );
    }

    // Les pions noirs.
    Bitboard pprise;
    // On ajoute le pion pouvant etre pris en passant
    // si c'est le cas.
    Bitboard temppieceb = cb.pieceb;
    if ( cb.EnPassantB[ ply ] >= 0 )
      temppieceb |= mask[cb.EnPassantB[ ply ]];
    // Prise par la droite.
    // On enleve la colonne H.
    piecebd = cb.pionn & 0xFEFEFEFEFEFEFEFELL;
    piecebd <<= 7;
    pprise = piecebd & temppieceb;
    // Prise par la gauche.
    piecebd = cb.pionn & 0x7F7F7F7F7F7F7F7FLL;
    piecebd <<= 9;
    pprise |= piecebd & temppieceb;
    while( pprise ) {
      move.To = DernierBit( pprise );
      move.Piece = pion;
      move.Capture = cb.board[move.To];
      move.Promotion = 0;
      if ( move.To % 8 != 7 && cb.board[move.To-7] == -pion ) {
        move.From = move.To - 7;
        if ( move.To > 55 ) {
          move.Promotion = dame;
          movelist.Ajoute( move );
          move.Promotion = tour;
          movelist.Ajoute( move );
          move.Promotion = fou;
          movelist.Ajoute( move );
          move.Promotion = cavalier;
          movelist.Ajoute( move );
        }
        else {
          move.Promotion = 0;
          if ( cb.board[move.To] != 0 ) {
            move.EnPassant = 0;
          }
          else {
            // En passant.
            move.Capture = pion;
            move.EnPassant = 1;
          }
          movelist.Ajoute( move );
        }
      }
      if ( move.To % 8 != 0 && cb.board[move.To-9] == -pion ) {
        move.From = move.To - 9;
        if ( move.To > 55 ) {
          move.Promotion = dame;
          movelist.Ajoute( move );
          move.Promotion = tour;
          movelist.Ajoute( move );
          move.Promotion = fou;
          movelist.Ajoute( move );
          move.Promotion = cavalier;
          movelist.Ajoute( move );
        }
        else {
          move.Promotion = 0;
          if ( cb.board[move.To] != 0 ) {
            move.EnPassant = 0;
          }
          else {
            // En passant.
            move.Capture = pion;
            move.EnPassant = 1;
          }
          movelist.Ajoute( move );
        }
      }
      ClearBit( pprise, move.To );
    }

    // Le roi noir.
    piecebd = cb.roin;
    move.EnPassant = 0;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (cb.AttaqueRoi[move.From] & cb.pieceb);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = roi;
        move.Capture = cb.board[move.To];
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }
  }
}

//-----------------------------------------------------------------------------
// Genere tout les moves possibles qui n'attaque pas.
//
void GenMovePasAttaque(int ply,                  // Profondeur.
                       int wtm,                 // Joueur
                       TMoveList& movelist )     // Liste des coups.
{
  Bitboard piecebd, temp;
  TMove move;
  memset( &move, 0, sizeof( TMove ) );

  // Les blancs.
  if ( wtm ) {

    // Le roque cote roi.
    if ( !(cb.Roque & ROQUEROIBLANC) &&
         cb.board[E1] == roi &&
         cb.board[H1] == tour && 
         (cb.vide & 0x6000000000000000LL) == 0x6000000000000000LL &&
         !Attacked(E1, !wtm) &&
         !Attacked(F1, !wtm) &&
         !Attacked(G1, !wtm) ) {
      move.From = E1; move.To = G1;
      move.Piece = roi;
      move.Roque = 1;
      movelist.Ajoute( move );
    }
    if ( !(cb.Roque & ROQUEDAMEBLANC) &&
         (cb.vide & 0x0E00000000000000LL) == 0x0E00000000000000LL &&
         cb.board[E1] == roi &&
         cb.board[A1] == tour &&
         !Attacked(E1, !wtm) &&
         !Attacked(D1, !wtm) &&
         !Attacked(C1, !wtm) ) {

      move.From = E1; move.To = C1;
      move.Piece = roi;
      move.Roque = 1;
      movelist.Ajoute( move );
    }

    // Les cavaliers blancs.
    piecebd = cb.cavalierb;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (cb.AttaqueCavalier[move.From] & cb.vide);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = cavalier;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les fous blancs.
    piecebd = cb.foub;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueFou(move.From)&cb.vide);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = fou;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les tours blanches.
    piecebd = cb.tourb;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueTour(move.From)&cb.vide);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = tour;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les dames blanches.
    piecebd = cb.dameb;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueDame(move.From)&cb.vide);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = dame;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    //------------------------------------------------------------
    // Les pions blancs.
    // On genere tous les coups possibles pour les pions.
    // Les coups seront genere en 2 coups.
    // 1- On shift de 8 les pions.
    // 2- On mask les pions de depart et on shift encore de 8.
    Bitboard pavance1 = (cb.pionb >> 8);
    pavance1 &= cb.vide;
    pavance1 &= 0x00FFFFFFFFFFFFFFLL;

    Bitboard pavance2 = (pavance1&cb.pionbdepart) >> 8;
    pavance2 &= cb.vide;

    // On prend chaque bit et on genere sont move.
    while( pavance2 ) {
      // Insertion du move.
      move.To = DernierBit( pavance2 );
      move.From = move.To+16;
      move.Piece = pion;
      move.Roque = 0;
      movelist.Ajoute( move );
      // On enleve le bit.
      ClearBit( pavance2, move.To );
    }

    while( pavance1 ) {
      // Insertion du move.
      move.To = DernierBit( pavance1 );
      move.From = move.To+8;
      move.Piece = pion;
      move.Roque = 0;
      if ( move.To < 8 ) {
        move.Promotion = dame;
        movelist.Ajoute( move );
        move.Promotion = tour;
        movelist.Ajoute( move );
        move.Promotion = fou;
        movelist.Ajoute( move );
        move.Promotion = cavalier;
        movelist.Ajoute( move );
      }
      else {
        move.Promotion = 0;
        movelist.Ajoute( move );
      }
      // On enleve le bit.
      ClearBit( pavance1, move.To );
    }

    // Le roi blanc.
    piecebd = cb.roib;
	move.Promotion = 0;
	move.Capture = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (cb.AttaqueRoi[move.From] & cb.vide);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = roi;
        move.Roque = 0;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }
  }
  //-----------------------------
  else {     // Les noirs.

    // Generer les roques possibles.
      // Le roque cote roi.
    if ( !(cb.Roque & ROQUEROINOIR) &&
         cb.board[E8] == -roi &&
         cb.board[H8] == -tour &&
         (cb.vide & 0x60 ) == 0x60 &&
         !Attacked(E8, !wtm) &&
         !Attacked(F8, !wtm) &&
         !Attacked(G8, !wtm) ) {
      move.From = E8; move.To = G8;
      move.Piece = roi;
      move.Roque = 1;
      movelist.Ajoute( move );
    }
    if ( !(cb.Roque & ROQUEDAMENOIR) &&
         cb.board[E8] == -roi &&
         cb.board[A8] == -tour &&
         (cb.vide & 0x0E) == 0x0E &&
         !Attacked(E8, !wtm ) &&
         !Attacked(D8, !wtm ) &&
         !Attacked(C8, !wtm ) ) {
      move.From = E8; move.To = C8;
      move.Piece = roi;
      move.Roque = 1;
      movelist.Ajoute( move );
    }

    // Les cavaliers noirs.
    piecebd = cb.cavaliern;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (cb.AttaqueCavalier[move.From]) & cb.vide;
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = cavalier;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les fous noirs.
    piecebd = cb.foun;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueFou(move.From)&cb.vide);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = fou;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les tours noires.
    piecebd = cb.tourn;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (AttaqueTour(move.From)&cb.vide);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = tour;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les dames noires.
    piecebd = cb.damen;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = AttaqueDame(move.From);
      temp &= cb.vide;
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = dame;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    //------------------------------------------------------------
    // On genere tous les coups possibles pour les pions.
    // On ne genere pas ici les prises.
    // Les coups seront generes en 2 coups.
    // 1- On shift de 8 les pions.
    // 2- On mask les pions de depart et on shift encore de 8.
    Bitboard pavance1 = (cb.pionn << 8);
    pavance1 &= cb.vide;
    pavance1 &= 0xFFFFFFFFFFFFFF00LL;
    Bitboard pavance2 = (pavance1&cb.pionndepart) << 8;
    pavance2 &= cb.vide;

    // On prend chaque bit et on genere sont move.
    while( pavance2 ) {
      // Insertion du move.
      move.To = DernierBit( pavance2 );
      move.From = move.To-16;
      move.Piece = pion;
      move.Roque = 0;
      movelist.Ajoute( move );
      // On enleve le bit.
      ClearBit( pavance2, move.To );
    }

    while( pavance1 ) {
      // Insertion du move.
      move.To = DernierBit( pavance1 );
      move.From = move.To-8;
      move.Piece = pion;
      move.Roque = 0;
      if ( move.To > 55 ) {
        move.Promotion = dame;
        movelist.Ajoute( move );
        move.Promotion = tour;
        movelist.Ajoute( move );
        move.Promotion = fou;
        movelist.Ajoute( move );
        move.Promotion = cavalier;
        movelist.Ajoute( move );
      }
      else {
        move.Promotion = 0;
        movelist.Ajoute( move );
      }
      // On enleve le bit.
      ClearBit( pavance1, move.To );
    }

    // Le roi noir.
    piecebd = cb.roin;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = DernierBit( piecebd );
      temp = (cb.AttaqueRoi[move.From] & cb.vide);
      while( temp ) {
        move.To = DernierBit( temp );
        move.Piece = roi;
        move.Roque = 0;
        movelist.Ajoute( move );
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }
  }
}
