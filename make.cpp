//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Make.cpp
// Debut: 5 novembre 1998.
//
//---------------------------------------------------------------------------
#include <stdio.h>

#include "board.h"
#include "make.h"
#include "utile.h"
#include "chess.h"
#include "journal.h"
#include "eval.h"

extern TJournal journal;

// On effectue un coup dans l'echiquier. On mettera a jour tout
// les structures necessaire.
void MakeMove(int ply, TMove& move, int wtm)
{
  // Pour le defaire.
  cb.Regle50CoupStore[cb.NoCoups] = cb.Regle50Coup;
  cb.Regle50Coup++;
  cb.Positions[cb.NoCoups] = cb.CleHachage;

	// Les coups joues.
	cb.ListeRoque[cb.NoCoups] = cb.Roque;
	cb.ListeMove[ cb.NoCoups ] = move;

  // Effectuer les changements pour toutes les pieces.
  MovePiece( cb.piece, move );
  MovePieceR90L( cb.pieceR90L, move );
  MovePieceR45L( cb.pieceR45L, move );
  MovePieceR45R( cb.pieceR45R, move );
  if ( wtm ) {
    MovePiece( cb.pieceb, move );
    // Si il y avait une piece noire, elle a ete capture.
    ClearBit( cb.piecen, move.To );
    cb.EnPassantB[ply+1] = -1;
  }
  else {
    MovePiece( cb.piecen, move );
    // Si il y avait une piece blanche, elle a ete capture.
    ClearBit( cb.pieceb, move.To );
    cb.EnPassantN[ply+1] = -1;
  }

  cb.board[move.From] = 0;

  switch( move.Piece ) {
    case pion :
    // On bouge un pion.
    // 1- On avance d'une ou deux case.
    // 3- On a une promotion.
    cb.Regle50Coup=0;
    if ( wtm ) {
      MovePiece( cb.pionb, move );
      cb.board[move.To] = pion;
      // On calcul le positonnel.
      cb.CleHachage ^= ClePionB[move.From];
      cb.CleHachage ^= ClePionB[move.To];
	  if (move.Promotion) {
		// On enl�ve un pion.
		ClearBit( cb.pionb, move.To );
		cb.CleHachage ^= ClePionB[move.From];
		cb.TotalPionBlanc--;
		cb.NbWhitePieces++;
		cb.board[move.To] = move.Promotion;
		switch( move.Promotion ) {
	        case dame:
			SetBit( cb.dameb, move.To );
			cb.CleHachage ^= CleDameB[move.To];
			// Blanc gagne une dame.
			cb.TotalDameBlanche++;
            cb.ScoreMaterielBlanc += (SCORE)VALDAME-VALPION;
			break;
		case tour:
			SetBit(cb.tourb, move.To);
			cb.CleHachage ^= CleTourB[move.To];
			// Blanc gagne une tour.
			cb.TotalTourBlanche++;
			cb.ScoreMaterielBlanc += (SCORE)VALTOUR-VALPION;
			break;
		case fou:
			SetBit(cb.foub, move.To);
			cb.CleHachage ^= CleFouB[move.To];
			// Blanc gagne une fou.
			cb.TotalFouBlanc++;
			cb.ScoreMaterielBlanc += (SCORE)VALFOU-VALPION;
			break;
		case cavalier:
			SetBit(cb.cavalierb, move.To);
			cb.CleHachage ^= CleCavalierB[move.To];
			// Blanc gagne une cavalier.
			cb.TotalCavalierBlanc++;
			cb.ScoreMaterielBlanc += (SCORE)VALCAVALIER-VALPION;
			break;
		}
      }
    }
    else {
      // Pions noirs
      MovePiece( cb.pionn, move );
      cb.board[move.To] = -pion;
      cb.CleHachage ^= ClePionN[move.From];
      cb.CleHachage ^= ClePionN[move.To];
	  if (move.Promotion) {
		  // Enlever un pion.
		  ClearBit( cb.pionn, move.To );
		  cb.board[move.To] = -(char)move.Promotion;
		  cb.CleHachage ^= ClePionN[move.From];
		  cb.TotalPionNoir--;
		  cb.NbBlackPieces++;
		  switch( move.Promotion ) {
			case dame:
			  SetBit( cb.damen, move.To );
			  cb.CleHachage ^= CleDameN[move.To];
			  // Noir gagne une dame.
			  cb.TotalDameNoire++;
			  cb.ScoreMaterielNoir += (SCORE)VALDAME-VALPION;
			  break;
			case tour:
			  SetBit( cb.tourn, move.To );
			  cb.CleHachage ^= CleTourN[move.To];
			  // Noir gagne une tour.
			  cb.TotalTourNoire++;
			  cb.ScoreMaterielNoir += (SCORE)VALTOUR-VALPION;
			  break;
			case fou:
			  SetBit( cb.foun, move.To );
			  cb.CleHachage ^= CleFouN[move.To];
			  // Noir gagne un fou.
			  cb.TotalFouNoir++;
			  cb.ScoreMaterielNoir += (SCORE)VALFOU-VALPION;
			  break;
			case cavalier:
			  SetBit( cb.cavaliern, move.To );
			  cb.CleHachage ^= CleCavalierN[move.To];
			  // Noir gagne un cavalier.
			  cb.TotalCavalierNoir++;
			  cb.ScoreMaterielNoir += (SCORE)VALCAVALIER-VALPION;
			  break;
		  }
	  }
    }
    break;

    // Les cavaliers.
    case cavalier:
      if ( wtm ) {
        MovePiece( cb.cavalierb, move );
        cb.board[move.To] = cavalier;
        cb.CleHachage ^= CleCavalierB[move.From];
        cb.CleHachage ^= CleCavalierB[move.To];
      }
      else {
        MovePiece( cb.cavaliern, move );
        cb.board[move.To] = -cavalier;
        cb.CleHachage ^= CleCavalierN[move.From];
        cb.CleHachage ^= CleCavalierN[move.To];
      }
      break;

    // Les fous.
    case fou:
      if ( wtm ) {
        MovePiece( cb.foub, move );
        cb.board[move.To] = fou;
        cb.CleHachage ^= CleFouB[move.From];
        cb.CleHachage ^= CleFouB[move.To];
      }
      else {
        MovePiece( cb.foun, move );
        cb.board[move.To] = -fou;
        cb.CleHachage ^= CleFouN[move.From];
        cb.CleHachage ^= CleFouN[move.To];
      }
      break;

    // Les tours.
    case tour:
      if ( wtm ) {
        MovePiece( cb.tourb, move );
        cb.board[move.To] = tour;
        cb.CleHachage ^= CleTourB[move.From];
        cb.CleHachage ^= CleTourB[move.To];
        if ( move.From == H1 ) {
          cb.Roque |= ROQUEROIBLANC;
        }
        else if ( move.From == A1 ) {
          cb.Roque |= ROQUEDAMEBLANC;
        }
      }
      else {
        MovePiece( cb.tourn, move );
        cb.board[move.To] = -tour;
        cb.CleHachage ^= CleTourN[move.From];
        cb.CleHachage ^= CleTourN[move.To];
        if ( move.From == H8 ) {
          cb.Roque |= ROQUEROINOIR;
        }
        else if ( move.From == A8 ) {
          cb.Roque |= ROQUEDAMENOIR;
        }
      }
      break;

    // Les dames.
    case dame:
      if ( wtm ) {
        MovePiece( cb.dameb, move );
        cb.board[move.To] = dame;
        cb.CleHachage ^= CleDameB[move.From];
        cb.CleHachage ^= CleDameB[move.To];
      }
      else {
        MovePiece( cb.damen, move );
        cb.board[move.To] = -dame;
        cb.CleHachage ^= CleDameN[move.From];
        cb.CleHachage ^= CleDameN[move.To];
      }
      break;

    // Les rois.
    case roi:
      if ( wtm ) {
        MovePiece( cb.roib, move );
        cb.board[move.To] = roi;
        cb.Roque |= ROQUEROIBLANC | ROQUEDAMEBLANC;
        cb.PositionRoiBlanc = move.To;
        cb.CleHachage ^= CleRoiB[move.From];
        cb.CleHachage ^= CleRoiB[move.To];
        // Verifier pour le roque.
        if ( move.From == E1 && (move.To == G1 || move.To == C1 )) {
           TMove movetour;
            // Deplacer la tour.
            // Cote roi.
            if ( move.To == G1 ) {
              movetour.From = H1; movetour.To = F1;
            }
            // Cote dame.
            else {
              movetour.From = A1; movetour.To = D1;
            }

            MovePiece( cb.piece, movetour );
            MovePieceR90L( cb.pieceR90L, movetour );
            MovePieceR45L( cb.pieceR45L, movetour );
            MovePieceR45R( cb.pieceR45R, movetour );
            MovePiece( cb.pieceb, movetour );
            MovePiece( cb.tourb, movetour );
            cb.board[movetour.To] = tour;
            cb.board[movetour.From] = 0;
            cb.CleHachage ^= CleTourB[movetour.From];
            cb.CleHachage ^= CleTourB[movetour.To];
        }
      }
      else {
        MovePiece( cb.roin, move );
        cb.board[move.To] = -roi;
        cb.Roque |= ROQUEROINOIR | ROQUEDAMENOIR;
        cb.PositionRoiNoir = move.To;
        cb.CleHachage ^= CleRoiN[move.From];
        cb.CleHachage ^= CleRoiN[move.To];
        if ( move.From == E8 && (move.To == G8 || move.To == C8 )) {
            TMove movetour;
            // Deplacer la tour.
            // Cote roi.
            if ( move.To == G8 ) {
              movetour.From = H8; movetour.To = F8;
            }
            else {
              movetour.From = A8; movetour.To = D8;
            }

            MovePiece( cb.piece, movetour );
            MovePieceR90L( cb.pieceR90L, movetour );
            MovePieceR45L( cb.pieceR45L, movetour );
            MovePieceR45R( cb.pieceR45R, movetour );
            MovePiece( cb.piecen, movetour );
            MovePiece( cb.tourn, movetour );
            cb.board[movetour.To] = -tour;
            cb.board[movetour.From] = 0;
            cb.CleHachage ^= CleTourN[movetour.From];
            cb.CleHachage ^= CleTourN[movetour.To];
        } // Roque.
      }
      break;
  }

  // Maintenant, verifier si une piece a ete capture et mettre a jour
  // en cas de besoin.
  if ( move.Capture ) {
    cb.Regle50Coup=0;
    switch( move.Capture ) {
      // Un pion.
      case pion :
        if ( wtm ) {
          // Noir perd un pion.
          cb.TotalPionNoir--;
          cb.TotalMaterielNoir--;
          cb.ScoreMaterielNoir -= VALPION;
          int to = move.To;
          if ( move.EnPassant ) {
            to += 8;
            cb.board[to] = 0;
            ClearBit( cb.piece, to );
            ClearBit( cb.piecen, to );
            ClearR90L( cb.pieceR90L, to );
            ClearR45L( cb.pieceR45L, to );
            ClearR45R( cb.pieceR45R, to );
          }
          cb.CleHachage ^= ClePionN[to];
          ClearBit( cb.pionn, to );
        }
        else {
          // Blanc perd un pion.
          cb.TotalPionBlanc--;
          cb.TotalMaterielBlanc--;
          cb.ScoreMaterielBlanc -= VALPION;
          int to = move.To;
          if ( move.EnPassant ) {
            to -= 8;
            cb.board[to] = 0;
            ClearBit( cb.piece, to );
            ClearBit( cb.pieceb, to );
            ClearR90L( cb.pieceR90L, to );
            ClearR45L( cb.pieceR45L, to );
            ClearR45R( cb.pieceR45R, to );
          }
          cb.CleHachage ^= ClePionB[to];
          ClearBit( cb.pionb, to );
        }
        break;
      // Un cavalier.
      case cavalier :
        if ( wtm ) {
          // Noir perd un cavalier.
          cb.TotalCavalierNoir--;
          cb.NbBlackPieces--;
          cb.TotalMaterielNoir--;
          cb.ScoreMaterielNoir -= VALCAVALIER;
          ClearBit( cb.cavaliern, move.To );
          cb.CleHachage ^= CleCavalierN[move.To];
        }
        else {
          // Blanc perd un cavalier.
          cb.TotalCavalierBlanc--;
          cb.NbWhitePieces--;
          cb.TotalMaterielBlanc--;
          cb.ScoreMaterielBlanc -= VALCAVALIER;
          ClearBit( cb.cavalierb, move.To );
          cb.CleHachage ^= CleCavalierB[move.To];
        }
        break;

      // Un fou.
      case fou :
        if ( wtm ) {
          // Noir perd un fou.
          cb.TotalFouNoir--;
          cb.NbBlackPieces--;
          cb.TotalMaterielNoir--;
          cb.ScoreMaterielNoir -= VALFOU;
          ClearBit( cb.foun, move.To );
          cb.CleHachage ^= CleFouN[move.To];
        }
        else {
          // Blanc perd un fou.
          cb.TotalFouBlanc--;
          cb.NbWhitePieces--;
          cb.TotalMaterielBlanc--;
          cb.ScoreMaterielBlanc -= VALFOU;
          ClearBit( cb.foub, move.To );
          cb.CleHachage ^= CleFouB[move.To];
        }
        break;

      // Une tour.
      case tour :
        if ( wtm ) {
          // Noir perd une tour.
          cb.TotalTourNoire--;
          cb.NbBlackPieces--;
          cb.TotalMaterielNoir--;
          cb.ScoreMaterielNoir -= VALTOUR;
          ClearBit( cb.tourn, move.To );
          ClearBit( cb.piecen, move.To );
          cb.CleHachage ^= CleTourN[move.To];
          if ( move.To == H8 ) {
            cb.Roque |= ROQUEROINOIR;
          }
          else
            if ( move.To == A8 ) {
            cb.Roque |= ROQUEDAMENOIR;
          }
        }
        else {
          // Blanc perd une tour.
          cb.TotalTourBlanche--;
          cb.NbWhitePieces--;
          cb.TotalMaterielBlanc--;
          cb.ScoreMaterielBlanc -= VALTOUR;
          ClearBit( cb.tourb, move.To );
          ClearBit( cb.pieceb, move.To );
          cb.CleHachage ^= CleTourB[move.To];
          if ( move.To == H1 ) {
            cb.Roque |= ROQUEROIBLANC;
          }
          else
            if ( move.To == A1 ) {
              cb.Roque |= ROQUEDAMEBLANC;
            }
        }
        break;

      // Une dame.
      case dame :
        if ( wtm ) {
          // Noir perd une dame.
          cb.TotalDameNoire--;
          cb.NbBlackPieces--;
          cb.TotalMaterielNoir--;
          cb.ScoreMaterielNoir -= VALDAME;
          ClearBit( cb.damen, move.To );
          cb.CleHachage ^= CleDameN[move.To];
        }
        else {
          // Blanc perd une dame.
          cb.TotalDameBlanche--;
          cb.NbWhitePieces--;
          cb.TotalMaterielBlanc--;
          cb.ScoreMaterielBlanc -= VALDAME;
          ClearBit( cb.dameb, move.To );
          cb.CleHachage ^= CleDameB[move.To];
        }
        break;
    }
  }

  // Verifier si genere une possibilite de en passant.
  if ( move.Piece == pion ) {
    if ( wtm ) {
      if ( move.From - move.To == 16 )
        cb.EnPassantB[ply+1] = move.To + 8;
    }
    else {
      if ( move.To - move.From == 16 )
        cb.EnPassantN[ply+1] = move.To - 8;
    }
  }

  cb.vide = ~cb.piece;
  cb.NoCoups++;
}

void MakeMoveRoot(TMove& move, int wtm)
{
	MakeMove(1, move, wtm);

	cb.EnPassantB[1] = cb.EnPassantB[2];
	cb.EnPassantN[1] = cb.EnPassantN[2];
}
