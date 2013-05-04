//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Attaque.cpp
// Debut: 10 novembre 1998.
//
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <stdio.h>

#include "utile.h"
#include "unmake.h"
#include "board.h"
#include "eval.h"
#include "sortie.h"
#include "journal.h"

extern TJournal journal;
extern int wtm;


//-----------------------------------------------------------------------------
// Routine pour defaire un coup.
void UnmakeMove(int ply, TMove& move, int wtm )
{
  cb.NoCoups--;
  cb.Regle50Coup = cb.Regle50CoupStore[cb.NoCoups];
  cb.CleHachage = cb.Positions[cb.NoCoups];

  // Pour commencer, remettre a sa place d'origine.
  SetBit( cb.piece, move.From );
  SetR90L( cb.pieceR90L, move.From );
  SetR45L( cb.pieceR45L, move.From );
  SetR45R( cb.pieceR45R, move.From );
  ClearBit( cb.piece, move.To );
  ClearR90L( cb.pieceR90L, move.To );
  ClearR45L( cb.pieceR45L, move.To );
  ClearR45R( cb.pieceR45R, move.To );
  if ( wtm ) {
    SetBit( cb.pieceb, move.From );
    ClearBit( cb.pieceb, move.To );
  }
  else {
    SetBit( cb.piecen, move.From );
    ClearBit( cb.piecen, move.To );
  }

  cb.board[move.To] = 0;

  // Maintenant, verifier chaque piece specifiquement pour mettre a jour
  // leur bitboards.
  switch( move.Piece ) {
    // Les pions.
    case pion:
      if ( wtm ) {
        SetBit( cb.pionb, move.From );
        ClearBit( cb.pionb, move.To );
        cb.board[move.From] = pion;
		if (move.Promotion) {
			cb.NbWhitePieces--;
			cb.TotalPionBlanc++;
			switch( move.Promotion ) {
			  case cavalier:
				cb.TotalCavalierBlanc--;
				cb.ScoreMaterielBlanc -= (SCORE)VALCAVALIER-VALPION;
				ClearBit( cb.cavalierb, move.To );
				break;
			  case fou:
				cb.TotalFouBlanc--;
				cb.ScoreMaterielBlanc -= (SCORE)VALFOU-VALPION;
				ClearBit( cb.foub, move.To );
				break;
			  case tour:
				cb.TotalTourBlanche--;
				cb.ScoreMaterielBlanc -= (SCORE)VALTOUR-VALPION;
				ClearBit( cb.tourb, move.To );
				break;
			  case dame:
				// Enlever le gain de la dame.
				cb.TotalDameBlanche--;
				cb.ScoreMaterielBlanc -= (SCORE)VALDAME-VALPION;
				// Rajouter le pion.
				ClearBit( cb.dameb, move.To );
				break;
			}
		}
      }
      else {
        SetBit( cb.pionn, move.From );
        ClearBit( cb.pionn, move.To );
        cb.board[move.From] = -pion;
		if (move.Promotion) {
			cb.NbBlackPieces--;
			cb.TotalPionNoir++;
			switch( move.Promotion ) {
			  case cavalier:
			    cb.TotalCavalierNoir--;
				cb.ScoreMaterielNoir -= (SCORE)VALCAVALIER-VALPION;
				ClearBit( cb.cavaliern, move.To );
				break;
			  case fou:
				cb.TotalFouNoir--;
				cb.ScoreMaterielNoir -= (SCORE)VALFOU-VALPION;
				ClearBit( cb.foun, move.To );
				break;
			  case tour:
			    cb.TotalTourNoire--;
				cb.ScoreMaterielNoir -= (SCORE)VALTOUR-VALPION;
				ClearBit( cb.tourn, move.To );
				break;
			  case dame:
				// Enlever le gain de la dame.
				cb.TotalDameNoire--;
				cb.ScoreMaterielNoir -= (SCORE)VALDAME-VALPION;
				ClearBit( cb.damen, move.To );
				break;
			}
		}
      } // if
      break;
    case cavalier:
      if ( wtm ) {
        SetBit( cb.cavalierb, move.From );
        ClearBit( cb.cavalierb, move.To );
        cb.board[move.From] = cavalier;
      }
      else {
        SetBit( cb.cavaliern, move.From );
        ClearBit( cb.cavaliern, move.To );
        cb.board[move.From] = -cavalier;
      }
      break;
    case fou:
      if ( wtm ) {
        SetBit( cb.foub, move.From );
        ClearBit( cb.foub, move.To );
        cb.board[move.From] = fou;
      }
      else {
        SetBit( cb.foun, move.From );
        ClearBit( cb.foun, move.To );
        cb.board[move.From] = -fou;
      }
      break;
    case tour:
      if ( wtm ) {
        SetBit( cb.tourb, move.From );
        ClearBit( cb.tourb, move.To );
        cb.board[move.From] = tour;
      }
      else {
        SetBit( cb.tourn, move.From );
        ClearBit( cb.tourn, move.To );
        cb.board[move.From] = -tour;
      }
      break;
    case dame:
      if ( wtm ) {
        SetBit( cb.dameb, move.From );
        ClearBit( cb.dameb, move.To );
        cb.board[move.From] = dame;
      }
      else {
        SetBit( cb.damen, move.From );
        ClearBit( cb.damen, move.To );
        cb.board[move.From] = -dame;
      }
      break;
    case roi:
      if ( wtm ) {
        SetBit( cb.roib, move.From );
        ClearBit( cb.roib, move.To );
        cb.board[move.From] = roi;
        cb.PositionRoiBlanc = move.From;
        // Le roque.
        if ( move.Roque ) {
          if ( move.From == E1 && ( move.To == G1 || move.To == C1 )) {
            TMove movetour;
            // Cote roi.
            if ( move.To == G1 ) {
              movetour.From = F1;
              movetour.To = H1;
            }
            // Cote dame.
            else {
              movetour.From = D1;
              movetour.To = A1;
            }
            MovePiece( cb.piece, movetour );
            MovePieceR90L( cb.pieceR90L, movetour );
            MovePieceR45L( cb.pieceR45L, movetour );
            MovePieceR45R( cb.pieceR45R, movetour );
            MovePiece( cb.pieceb, movetour );
            MovePiece( cb.tourb, movetour );
            cb.board[movetour.To] = tour;
            cb.board[movetour.From] = 0;
          }
        } // Roque.
      }
      else {
        SetBit( cb.roin, move.From );
        ClearBit( cb.roin, move.To );
        cb.board[move.From] = -roi;
        cb.PositionRoiNoir = move.From;
        // Le roque.
        if ( move.Roque ) {
          if ( move.From == E8 && ( move.To == G8 || move.To == C8 )) {
            TMove movetour;
            // Cote roi.
            if ( move.To == G8 ) {
              movetour.From = F8;
              movetour.To = H8;
            }
            // Cote dame.
            else {
              movetour.From = D8;
              movetour.To = A8;
            }
            MovePiece( cb.piece, movetour );
            MovePieceR90L( cb.pieceR90L, movetour );
            MovePieceR45L( cb.pieceR45L, movetour );
            MovePieceR45R( cb.pieceR45R, movetour );
            MovePiece( cb.piecen, movetour );
            MovePiece( cb.tourn, movetour );
            cb.board[movetour.To] = -tour;
            cb.board[movetour.From] = 0;
          }
        }
      }
      break;
  } // Switch

  switch( move.Capture ) {
    case pion:
        if ( wtm ) {
          // Noir reprend leur pion.
          cb.TotalPionNoir++;
          cb.TotalMaterielNoir++;
          cb.ScoreMaterielNoir += (SCORE)VALPION;
          int to = move.To;
          if ( move.EnPassant ) {
            to += 8;
          }
          SetBit( cb.piece, to );
          SetBit( cb.piecen, to );
          SetR90L( cb.pieceR90L, to );
          SetR45L( cb.pieceR45L, to );
          SetR45R( cb.pieceR45R, to );
          cb.board[to] = -pion;
          SetBit( cb.pionn, to );
        }
        else {
          // Blanc reprend leur pion.
          cb.TotalPionBlanc++;
          cb.TotalMaterielBlanc++;
          cb.ScoreMaterielBlanc += (SCORE)VALPION;
          int to = move.To;
          if ( move.EnPassant ) {
            to -= 8;
          }
          SetBit( cb.piece, to );
          SetBit( cb.pieceb, to );
          SetR90L( cb.pieceR90L, to );
          SetR45L( cb.pieceR45L, to );
          SetR45R( cb.pieceR45R, to );
          cb.board[to] = pion;
          SetBit( cb.pionb, to );
        }
      break;
    case cavalier:
      SetBit( cb.piece, move.To );
      SetR90L( cb.pieceR90L, move.To );
      SetR45L( cb.pieceR45L, move.To );
      SetR45R( cb.pieceR45R, move.To );
      if ( wtm ) {
        // Noir reprend leur cavalier.
        cb.TotalCavalierNoir++;
        cb.NbBlackPieces++;
        cb.TotalMaterielNoir++;
        cb.ScoreMaterielNoir += (SCORE)VALCAVALIER;
        SetBit( cb.cavaliern, move.To );
        SetBit( cb.piecen, move.To );
        cb.board[move.To] = -cavalier;
      }
      else {
        // Blanc reprend leur cavalier.
        cb.TotalCavalierBlanc++;
        cb.NbWhitePieces++;
        cb.TotalMaterielBlanc++;
        cb.ScoreMaterielBlanc += (SCORE)VALCAVALIER;
        SetBit( cb.cavalierb, move.To );
        SetBit( cb.pieceb, move.To );
        cb.board[move.To] = cavalier;
      }
      break;
    case fou:
      SetBit( cb.piece, move.To );
      SetR90L( cb.pieceR90L, move.To );
      SetR45L( cb.pieceR45L, move.To );
      SetR45R( cb.pieceR45R, move.To );
      if ( wtm ) {
        // Noir reprend leur fou.
        cb.TotalFouNoir++;
        cb.NbBlackPieces++;
        cb.TotalMaterielNoir++;
        cb.ScoreMaterielNoir += (SCORE)VALFOU;
        SetBit( cb.foun, move.To );
        SetBit( cb.piecen, move.To );
        cb.board[move.To] = -fou;
      }
      else {
        // Blanc reprend leur fou.
        cb.TotalFouBlanc++;
        cb.NbWhitePieces++;
        cb.TotalMaterielBlanc++;
        cb.ScoreMaterielBlanc += (SCORE)VALFOU;
        SetBit( cb.foub, move.To );
        SetBit( cb.pieceb, move.To );
        cb.board[move.To] = fou;
      }
      break;
    case tour:
      SetBit( cb.piece, move.To );
      SetR90L( cb.pieceR90L, move.To );
      SetR45L( cb.pieceR45L, move.To );
      SetR45R( cb.pieceR45R, move.To );
      if ( wtm ) {
        // Noir reprend tour.
        cb.TotalTourNoire++;
        cb.NbBlackPieces++;
        cb.TotalMaterielNoir++;
        cb.ScoreMaterielNoir += (SCORE)VALTOUR;
        SetBit( cb.tourn, move.To );
        SetBit( cb.piecen, move.To );
        cb.board[move.To] = -tour;
      }
      else {
        // Blanc reprend tour.
        cb.TotalTourBlanche++;
        cb.NbWhitePieces++;
        cb.TotalMaterielBlanc++;
        cb.ScoreMaterielBlanc += (SCORE)VALTOUR;
        SetBit( cb.tourb, move.To );
        SetBit( cb.pieceb, move.To );
        cb.board[move.To] = tour;
      }
      break;
    case dame:
      SetBit( cb.piece, move.To );
      SetR90L( cb.pieceR90L, move.To );
      SetR45L( cb.pieceR45L, move.To );
      SetR45R( cb.pieceR45R, move.To );
      if ( wtm ) {
        // Noir reprend dame.
        cb.TotalDameNoire++;
        cb.NbBlackPieces++;
        cb.TotalMaterielNoir++;
        cb.ScoreMaterielNoir += (SCORE)VALDAME;
        SetBit( cb.damen, move.To );
        SetBit( cb.piecen, move.To );
        cb.board[move.To] = -dame;
      }
      else {
        // Blanc reprend dame.
        cb.TotalDameBlanche++;
        cb.NbWhitePieces++;
        cb.TotalMaterielBlanc++;
        cb.ScoreMaterielBlanc += (SCORE)VALDAME;
        SetBit( cb.dameb, move.To );
        SetBit( cb.pieceb, move.To );
        cb.board[move.To] = dame;
      }
      break;
  } // Switch.

  cb.vide = ~cb.piece;
  cb.Roque = cb.ListeRoque[cb.NoCoups];
}

void UndoMove()
{
	wtm = !wtm;
	UnmakeMove(1, cb.ListeMove[cb.NoCoups-1], wtm);
}
