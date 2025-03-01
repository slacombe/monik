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
void genMoveAttaque(TChessBoard *cb,
                    int ply,                  // Profondeur.
                    int wtm,                 // Joueur
                    LINE& movelist)     // Liste des coups.
{
  Bitboard piecebd, temp;
  MOVE move;
  move.Score = 0;
  move.Roque = 0;

  // Les blancs.
  if ( wtm ) {

    // Les cavaliers blancs.
    move.EnPassant = 0;
    move.Promotion = 0;
    piecebd = cb->cavalierb;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (cb->AttaqueCavalier[move.From] & cb->piecen);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = cavalier;
        move.Capture = -cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les tours blanches.
    piecebd = cb->tourb;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueTour(cb, move.From)&cb->piecen);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = tour;
        move.Capture = -cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les dames blanches.
    piecebd = cb->dameb;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueDame(cb, move.From)&cb->piecen);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = dame;
        move.Capture = -cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les fous blancs.
    piecebd = cb->foub;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueFou(cb, move.From)&cb->piecen);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = fou;
        move.Capture = -cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    //------------------------------------------------------------
    // Les pions blancs.
    // Les promotions.
    Bitboard pavance1 = (cb->pionb >> 8);
    pavance1 &= cb->vide;
    pavance1 &= 0xFF00000000000000LL;

    // On prend chaque bit et on genere sont move.
    while( pavance1 ) {
      // Insertion du move.
      move.To = dernierBit( pavance1 );
      move.From = move.To+8;
      move.Piece = pion;
      ajouteMove(&movelist, move);
      // On enleve le bit.
      ClearBit( pavance1, move.To );
    }
    // Les pions blancs.
    Bitboard pprise;
    // On ajoute le pion pouvant etre pris en passant
    // si c'est le cas.
    Bitboard temppiecen = cb->piecen;
    if ( cb->EnPassant[ply] >= 0 )
      temppiecen |= mask[cb->EnPassant[ply]];
    // Prise par la droite.
    // On enleve la colonne H.
    piecebd = cb->pionb & 0xFEFEFEFEFEFEFEFELL;
    piecebd >>= 9;
    pprise = piecebd & temppiecen;
    // Prise par la gauche.
    piecebd = cb->pionb & 0x7F7F7F7F7F7F7F7FLL;
    piecebd >>= 7;
    pprise |= piecebd & temppiecen;
    while( pprise ) {
      move.To = dernierBit( pprise );
      move.Piece = pion;
      move.Capture = -cb->board[move.To];
      if ( move.To % 8 != 0 && cb->board[move.To+7] == pion ) {
        move.From = move.To + 7;
        if ( move.To < 8 ) {
          // Une promotion.
          move.Promotion = dame;
          ajouteMove(&movelist, move);
          move.Promotion = tour;
          ajouteMove(&movelist, move);
          move.Promotion = fou;
          ajouteMove(&movelist, move);
          move.Promotion = cavalier;
          ajouteMove(&movelist, move);
        }
        else {
          move.Promotion = 0;
          if ( cb->board[move.To] != 0 ) {
            move.EnPassant = 0;
          }
          else {
            // En passant.
            move.Capture = pion;
            move.EnPassant = 1;
          }
          ajouteMove(&movelist, move);
        }
      }
      if ( move.To % 8 != 7 && cb->board[move.To+9] == pion ) {
        move.From = move.To + 9;
        if ( move.To < 8 ) {
          // Une promotion.
          move.Promotion = dame;
          ajouteMove(&movelist, move);
          move.Promotion = tour;
          ajouteMove(&movelist, move);
          move.Promotion = fou;
          ajouteMove(&movelist, move);
          move.Promotion = cavalier;
          ajouteMove(&movelist, move);
        }
        else {
          if ( cb->board[move.To] != 0 ) {
            move.EnPassant = 0;
          }
          else {
            // En passant.
            move.Capture = pion;
            move.EnPassant = 1;
          }
          move.Promotion = 0;
          ajouteMove(&movelist, move);
        }
      }
      ClearBit( pprise, move.To );
    }

    // Le roi blancs.
    piecebd = cb->roib;
    move.EnPassant = 0;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (cb->AttaqueRoi[move.From] & cb->piecen);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = roi;
        move.Capture = -cb->board[move.To];
        ajouteMove(&movelist, move);
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
    piecebd = cb->cavaliern;
    move.EnPassant = 0;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (cb->AttaqueCavalier[move.From]) & cb->pieceb;
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = cavalier;
        move.Capture = cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les dames noires.
    piecebd = cb->damen;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueDame(cb, move.From)&cb->pieceb);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = dame;
        move.Capture = cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les tours noirs.
    piecebd = cb->tourn;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueTour(cb, move.From)&cb->pieceb);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = tour;
        move.Capture = cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les fous noirs.
    piecebd = cb->foun;
    move.EnPassant = 0;
    move.Promotion = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueFou(cb, move.From) & cb->pieceb);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = fou;
        move.Capture = cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    //------------------------------------------------------------
    // Les promotions.
    Bitboard pavance1 = (cb->pionn << 8);
    pavance1 &= cb->vide;
    pavance1 &= 0x00000000000000FF;
    
    // On prend chaque bit et on genere sont move.
    while( pavance1 ) {
      // Insertion du move.
      move.To = dernierBit( pavance1 );
      move.From = move.To-8;
      move.Piece = pion;
      ajouteMove(&movelist, move);
      // On enleve le bit.
      ClearBit( pavance1, move.To );
    }

    // Les pions noirs.
    Bitboard pprise;
    // On ajoute le pion pouvant etre pris en passant
    // si c'est le cas.
    Bitboard temppieceb = cb->pieceb;
    if ( cb->EnPassant[ ply ] >= 0 )
      temppieceb |= mask[cb->EnPassant[ply]];
    // Prise par la droite.
    // On enleve la colonne H.
    piecebd = cb->pionn & 0xFEFEFEFEFEFEFEFELL;
    piecebd <<= 7;
    pprise = piecebd & temppieceb;
    // Prise par la gauche.
    piecebd = cb->pionn & 0x7F7F7F7F7F7F7F7FLL;
    piecebd <<= 9;
    pprise |= piecebd & temppieceb;
    while( pprise ) {
      move.To = dernierBit( pprise );
      move.Piece = pion;
      move.Capture = cb->board[move.To];
      move.Promotion = 0;
      if ( move.To % 8 != 7 && cb->board[move.To-7] == -pion ) {
        move.From = move.To - 7;
        if ( move.To > 55 ) {
          move.Promotion = dame;
          ajouteMove(&movelist, move);
          move.Promotion = tour;
          ajouteMove(&movelist, move);
          move.Promotion = fou;
          ajouteMove(&movelist, move);
          move.Promotion = cavalier;
          ajouteMove(&movelist, move);
        }
        else {
          move.Promotion = 0;
          if ( cb->board[move.To] != 0 ) {
            move.EnPassant = 0;
          }
          else {
            // En passant.
            move.Capture = pion;
            move.EnPassant = 1;
          }
          ajouteMove(&movelist, move);
        }
      }
      if ( move.To % 8 != 0 && cb->board[move.To-9] == -pion ) {
        move.From = move.To - 9;
        if ( move.To > 55 ) {
          move.Promotion = dame;
          ajouteMove(&movelist, move);
          move.Promotion = tour;
          ajouteMove(&movelist, move);
          move.Promotion = fou;
          ajouteMove(&movelist, move);
          move.Promotion = cavalier;
          ajouteMove(&movelist, move);
        }
        else {
          move.Promotion = 0;
          if ( cb->board[move.To] != 0 ) {
            move.EnPassant = 0;
          }
          else {
            // En passant.
            move.Capture = pion;
            move.EnPassant = 1;
          }
          ajouteMove(&movelist, move);
        }
      }
      ClearBit( pprise, move.To );
    }

    // Le roi noir.
    piecebd = cb->roin;
    move.EnPassant = 0;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (cb->AttaqueRoi[move.From] & cb->pieceb);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = roi;
        move.Capture = cb->board[move.To];
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }
  }
}

//-----------------------------------------------------------------------------
// Genere tout les moves possibles qui n'attaque pas.
//
void genMovePasAttaque(TChessBoard *cb,
			int ply,                  // Profondeur.
                	int wtm,                 // Joueur
                	LINE& movelist)     // Liste des coups.
{
  Bitboard piecebd, temp;
  MOVE move;
  memset(&move, 0, sizeof( MOVE ));

  // Les blancs.
  if ( wtm ) {

    // Le roque cote roi.
    if ( !(cb->Roque & ROQUEROIBLANC) &&
         cb->board[E1] == roi &&
         cb->board[H1] == tour && 
         (cb->vide & 0x6000000000000000LL) == 0x6000000000000000LL &&
         !attacked(cb, E1, !wtm) &&
         !attacked(cb, F1, !wtm) &&
         !attacked(cb, G1, !wtm) ) {
      move.From = E1; move.To = G1;
      move.Piece = roi;
      move.Roque = 1;
      ajouteMove(&movelist, move);
    }
    if ( !(cb->Roque & ROQUEDAMEBLANC) &&
         (cb->vide & 0x0E00000000000000LL) == 0x0E00000000000000LL &&
         cb->board[E1] == roi &&
         cb->board[A1] == tour &&
         !attacked(cb, E1, !wtm) &&
         !attacked(cb, D1, !wtm) &&
         !attacked(cb, C1, !wtm) ) {

      move.From = E1; move.To = C1;
      move.Piece = roi;
      move.Roque = 1;
      ajouteMove(&movelist, move);
    }

    // Les cavaliers blancs.
    piecebd = cb->cavalierb;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (cb->AttaqueCavalier[move.From] & cb->vide);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = cavalier;
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les fous blancs.
    piecebd = cb->foub;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueFou(cb, move.From) & cb->vide);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = fou;
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les tours blanches.
    piecebd = cb->tourb;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueTour(cb, move.From) & cb->vide);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = tour;
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les dames blanches.
    piecebd = cb->dameb;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueDame(cb, move.From) & cb->vide);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = dame;
        ajouteMove(&movelist, move);
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
    Bitboard pavance1 = (cb->pionb >> 8);
    pavance1 &= cb->vide;
    pavance1 &= 0x00FFFFFFFFFFFFFFLL;

    Bitboard pavance2 = (pavance1 & cb->pionbdepart) >> 8;
    pavance2 &= cb->vide;

    // On prend chaque bit et on genere sont move.
    while( pavance2 ) {
      // Insertion du move.
      move.To = dernierBit( pavance2 );
      move.From = move.To+16;
      move.Piece = pion;
      move.Roque = 0;
      ajouteMove(&movelist, move);
      // On enleve le bit.
      ClearBit( pavance2, move.To );
    }

    while( pavance1 ) {
      // Insertion du move.
      move.To = dernierBit( pavance1 );
      move.From = move.To+8;
      move.Piece = pion;
      move.Roque = 0;
      if ( move.To < 8 ) {
        move.Promotion = dame;
        ajouteMove(&movelist, move);
        move.Promotion = tour;
        ajouteMove(&movelist, move);
        move.Promotion = fou;
        ajouteMove(&movelist, move);
        move.Promotion = cavalier;
        ajouteMove(&movelist, move);
      }
      else {
        move.Promotion = 0;
        ajouteMove(&movelist, move);
      }
      // On enleve le bit.
      ClearBit( pavance1, move.To );
    }

    // Le roi blanc.
    piecebd = cb->roib;
	move.Promotion = 0;
	move.Capture = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (cb->AttaqueRoi[move.From] & cb->vide);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = roi;
        move.Roque = 0;
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }
  }
  //-----------------------------
  else {     // Les noirs.

    // Generer les roques possibles.
      // Le roque cote roi.
    if ( !(cb->Roque & ROQUEROINOIR) &&
         cb->board[E8] == -roi &&
         cb->board[H8] == -tour &&
         (cb->vide & 0x60 ) == 0x60 &&
         !attacked(cb, E8, !wtm) &&
         !attacked(cb, F8, !wtm) &&
         !attacked(cb, G8, !wtm) ) {
      move.From = E8; move.To = G8;
      move.Piece = roi;
      move.Roque = 1;
      ajouteMove(&movelist, move);
    }
    if ( !(cb->Roque & ROQUEDAMENOIR) &&
         cb->board[E8] == -roi &&
         cb->board[A8] == -tour &&
         (cb->vide & 0x0E) == 0x0E &&
         !attacked(cb, E8, !wtm ) &&
         !attacked(cb, D8, !wtm ) &&
         !attacked(cb, C8, !wtm ) ) {
      move.From = E8; move.To = C8;
      move.Piece = roi;
      move.Roque = 1;
      ajouteMove(&movelist, move);
    }

    // Les cavaliers noirs.
    piecebd = cb->cavaliern;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (cb->AttaqueCavalier[move.From]) & cb->vide;
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = cavalier;
        ajouteMove(&movelist, move);
        ClearBit(temp, move.To);
      }
      ClearBit( piecebd, move.From );
    }

    // Les fous noirs.
    piecebd = cb->foun;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueFou(cb, move.From) & cb->vide);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = fou;
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les tours noires.
    piecebd = cb->tourn;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (attaqueTour(cb, move.From) & cb->vide);
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = tour;
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }

    // Les dames noires.
    piecebd = cb->damen;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = attaqueDame(cb, move.From);
      temp &= cb->vide;
      while( temp ) {
        move.To = dernierBit( temp );
        move.Piece = dame;
        ajouteMove(&movelist, move);
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
    Bitboard pavance1 = (cb->pionn << 8);
    pavance1 &= cb->vide;
    pavance1 &= 0xFFFFFFFFFFFFFF00LL;
    Bitboard pavance2 = (pavance1 & cb->pionndepart) << 8;
    pavance2 &= cb->vide;

    // On prend chaque bit et on genere sont move.
    while( pavance2 ) {
      // Insertion du move.
      move.To = dernierBit( pavance2 );
      move.From = move.To-16;
      move.Piece = pion;
      move.Roque = 0;
      ajouteMove(&movelist, move);
      // On enleve le bit.
      ClearBit( pavance2, move.To );
    }

    while( pavance1 ) {
      // Insertion du move.
      move.To = dernierBit( pavance1 );
      move.From = move.To-8;
      move.Piece = pion;
      move.Roque = 0;
      if ( move.To > 55 ) {
        move.Promotion = dame;
        ajouteMove(&movelist, move);
        move.Promotion = tour;
        ajouteMove(&movelist, move);
        move.Promotion = fou;
        ajouteMove(&movelist, move);
        move.Promotion = cavalier;
        ajouteMove(&movelist, move);
      }
      else {
        move.Promotion = 0;
        ajouteMove(&movelist, move);
      }
      // On enleve le bit.
      ClearBit( pavance1, move.To );
    }

    // Le roi noir.
    piecebd = cb->roin;
    move.Promotion = 0;
    move.Roque = 0;
    while( piecebd ) {
      move.From = dernierBit( piecebd );
      temp = (cb->AttaqueRoi[move.From] & cb->vide);
      while(temp) {
        move.To = dernierBit( temp );
        move.Piece = roi;
        move.Roque = 0;
        ajouteMove(&movelist, move);
        ClearBit( temp, move.To );
      }
      ClearBit( piecebd, move.From );
    }
  }
}
