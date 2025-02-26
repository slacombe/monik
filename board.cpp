//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Board.cpp
// Debut: 2 novembre 1998.
//
//---------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "chess.h"
#include "eval.h"
#include "init.h"
#include "make.h"
#include "unmake.h"
#include "utile.h"
#include "log.h"
#include "sortie.h"

using namespace std;

TMove pv[MAXPLY][MAXPLY];
int pv_length[MAXPLY];

TChessBoard *createPosition() { return new TChessBoard; }

void initializeChessBoard(TChessBoard *cb) {
  initialiseData();

  // Initialise le board.
  initialiseBoard(cb);

  // Initialise les bitboards.
  initialiseBitboard(cb);
}

void initialiseBoard(TChessBoard *cb) {
  // On met toute notre structure a zero.
  memset(cb, 0, sizeof(TChessBoard));

  // Les pions.
  int i;
  for (i = A7; i <= H7; i++)
    cb->board[i] = -pion; // Noirs.
  for (i = A2; i <= H2; i++)
    cb->board[i] = pion; // Blancs.

  // Les cavaliers.
  cb->board[B1] = cavalier;
  cb->board[G1] = cavalier;
  cb->board[B8] = -cavalier;
  cb->board[G8] = -cavalier;

  // Les fous.
  cb->board[C1] = fou;
  cb->board[F1] = fou;
  cb->board[C8] = -fou;
  cb->board[F8] = -fou;

  // Les tours.
  cb->board[A1] = tour;
  cb->board[H1] = tour;
  cb->board[A8] = -tour;
  cb->board[H8] = -tour;

  // Les dames.
  cb->board[D1] = dame;
  cb->board[D8] = -dame;

  // Les rois.
  cb->board[E1] = roi;
  cb->board[E8] = -roi;

  cb->PositionRoiBlanc = E1;
  cb->PositionRoiNoir = E8;
}

void initialiseBitboard(TChessBoard *cb) {
  // On initialise la table des masks.
  // On utilise normalement cette table en combinaison avec une autre pour
  // determiner certaine condition.
  Bitboard b = 1;
  int i, j, k;
  for (i = A8; i <= H1; i++)
    mask[i] = (b << i);

  // Pour chaque piece sur le jeu on initialise ses bitboards.
  for (i = A8; i <= H1; i++) {
    switch (cb->board[i]) {
    // Pion blanc.
    case pion:
      cb->pionb |= mask[i];
      cb->TotalPionBlanc++;
      cb->TotalMaterielBlanc++;
      cb->ScoreMaterielBlanc += VALPION;
      cb->CleHachage ^= ClePionB[i];
      break;
    case -pion:
      cb->pionn |= mask[i];
      cb->TotalPionNoir++;
      cb->TotalMaterielNoir++;
      cb->ScoreMaterielNoir += VALPION;
      cb->CleHachage ^= ClePionN[i];
      break;
    case cavalier:
      cb->cavalierb |= mask[i];
      cb->TotalCavalierBlanc++;
      cb->TotalMaterielBlanc++;
      cb->NbWhitePieces++;
      cb->ScoreMaterielBlanc += VALCAVALIER;
      cb->CleHachage ^= CleCavalierB[i];
      break;
    case -cavalier:
      cb->cavaliern |= mask[i];
      cb->TotalCavalierNoir++;
      cb->TotalMaterielNoir++;
      cb->NbBlackPieces++;
      cb->ScoreMaterielNoir += VALCAVALIER;
      cb->CleHachage ^= CleCavalierN[i];
      break;
    case fou:
      cb->foub |= mask[i];
      cb->TotalFouBlanc++;
      cb->TotalMaterielBlanc++;
      cb->NbWhitePieces++;
      cb->ScoreMaterielBlanc += VALFOU;
      cb->CleHachage ^= CleFouB[i];
      break;
    case -fou:
      cb->foun |= mask[i];
      cb->TotalFouNoir++;
      cb->TotalMaterielNoir++;
      cb->NbBlackPieces++;
      cb->ScoreMaterielNoir += VALFOU;
      cb->CleHachage ^= CleFouN[i];
      break;
    case tour:
      cb->tourb |= mask[i];
      cb->TotalTourBlanche++;
      cb->TotalMaterielBlanc++;
      cb->NbWhitePieces++;
      cb->ScoreMaterielBlanc += VALTOUR;
      cb->CleHachage ^= CleTourB[i];
      break;
    case -tour:
      cb->tourn |= mask[i];
      cb->TotalTourNoire++;
      cb->TotalMaterielNoir++;
      cb->NbBlackPieces++;
      cb->ScoreMaterielNoir += VALTOUR;
      cb->CleHachage ^= CleTourN[i];
      break;
    case dame:
      cb->dameb |= mask[i];
      cb->TotalDameBlanche++;
      cb->TotalMaterielBlanc++;
      cb->NbWhitePieces++;
      cb->ScoreMaterielBlanc += VALDAME;
      cb->CleHachage ^= CleDameB[i];
      break;
    case -dame:
      cb->damen |= mask[i];
      cb->TotalDameNoire++;
      cb->TotalMaterielNoir++;
      cb->NbBlackPieces++;
      cb->ScoreMaterielNoir += VALDAME;
      cb->CleHachage ^= CleDameN[i];
      break;
    case roi:
      cb->roib |= mask[i];
      cb->PositionRoiBlanc = i;
      cb->CleHachage ^= CleRoiB[i];
      break;
    case -roi:
      cb->roin |= mask[i];
      cb->PositionRoiNoir = i;
      cb->CleHachage ^= CleRoiN[i];
      break;
    }
    if (cb->board[i]) {
      cb->piece |= mask[i];
      cb->pieceR90L |= mask[Rotate90LMap[i]];
      cb->pieceR45L |= mask[Rotate45LRealMap[i]];
      cb->pieceR45R |= mask[Rotate45RRealMap[i]];

      if (cb->board[i] > 0) {
        cb->pieceb |= mask[i];
      } else {
        cb->piecen |= mask[i];
      }
    }

  } // for

  // Les cases vides.
  cb->vide = ~cb->piece;

  // Les masks de departs des pions. Pour voir si un pion etait a sa case
  // depart afin de pouvoir genere l'avancement de 2 cases.
  cb->pionndepart = 0x0000000000FF0000LL;
  cb->pionbdepart = 0x0000FF0000000000LL;

  // Les attaques possibles pour chaque pieces.
  // Les pions blancs.
  for (i = A7; i <= H1; i++) {
    Bitboard piecebd = 0;
    if (i % 8 != 0)
      piecebd = mask[i - 9];
    if (i % 8 != 7)
      piecebd |= mask[i - 7];
    cb->AttaquePionBlanc[i] = piecebd;
  }

  // Les pions noirs.
  for (i = A8; i <= H2; i++) {
    Bitboard piecebd = 0;
    if (i % 8 != 0)
      piecebd = mask[i + 7];
    if (i % 8 != 7)
      piecebd |= mask[i + 9];
    cb->AttaquePionNoir[i] = piecebd;
  }
  // Les cavaliers.
  int moveCavalier[] = {17, 15, 10, 6, -6, -10, -15, -17};
  for (i = A8; i <= H1; i++) {
    int iRangeeS = i / 8;
    int iColonneS = i & 7;
    for (j = 0; j < 8; j++) {
      int iDest = i + moveCavalier[j];
      if (iDest < 0 || iDest > 63)
        continue;
      int iRangeeD = iDest / 8;
      int iColonneD = iDest & 7;
      if (abs(iRangeeS - iRangeeD) > 2 || abs(iColonneS - iColonneD) > 2)
        continue;
      cb->AttaqueCavalier[i] |= mask[i + moveCavalier[j]];
    }
  }

  // Les bitboards d'attaque du roi.
  int moveRoi[] = {9, 8, 7, 1, -1, -7, -8, -9};
  for (i = A8; i <= H1; i++) {
    int iRangeeS = i / 8;
    int iColonneS = i & 7;
    for (int j = 0; j < 8; j++) {
      int iDest = i + moveRoi[j];
      if (iDest < 0 || iDest > 63)
        continue;
      int iRangeeD = iDest / 8;
      int iColonneD = iDest & 7;
      if (abs(iRangeeS - iRangeeD) > 1 || abs(iColonneS - iColonneD) > 1)
        continue;
      cb->AttaqueRoi[i] |= mask[i + moveRoi[j]];
    }
  }

  // Les bitboards d'attaque des rangee et colonnes.
  // Pour les dames et les tours.
  unsigned char cMaskPosR = 0x01, cRangeeMask[8][256];
  for (i = 0; i < 8; i++) {         // Pour chaque position sur la rangee.
    for (int j = 0; j < 256; j++) { // Pour toute les combinaisons possibles.
      // On decale le bit vers la droite jusqu'a ce qu'on rencontre un bit
      // ou la fin.
      unsigned char cTempMaskR = cMaskPosR, cMaskAttR = 0;
      do {
        cTempMaskR >>= 1;
        cMaskAttR |= cTempMaskR;
      } while (!(cTempMaskR & j) && cTempMaskR != 0);

      // La meme chose vers la gauche.
      cTempMaskR = cMaskPosR;
      do {
        cTempMaskR <<= 1;
        cMaskAttR |= cTempMaskR;
      } while (!(cTempMaskR & j) && (cTempMaskR & 0xFF) != 0);

      // On le place dans l'index.
      cRangeeMask[i][j] = cMaskAttR;
    }

    // Prochaine position.
    cMaskPosR <<= 1;
  }

  // Maintenant, on prend les rangees et on les inseres dans les
  // bitboards d'attaque de rangee.
  for (i = 0; i < 8; i++) {   // Pour les 8 rangees du board.
    for (j = 0; j < 8; j++) { // Pour chaque position dans la rangee.
      for (k = 0; k < 256; k++) {
        Bitboard temp = cRangeeMask[j][k];
        temp <<= i * 8;
        cb->AttRangee[i * 8 + j][k] = temp;
      }
    }
  }

  // Maintenant, prendre les rangees et on les inseres dans les
  // bitboards d'attaque de colonne.
  for (i = 0; i < 8; i++) {   // Pour le 8 colonnes du board.
    for (j = 0; j < 8; j++) { // Pour chaque position dans la colonne.
      for (k = 0; k < 256; k++) {
        // Ici, prendre la rangee, la mettre dans le bitboard, la decaler
        // et ensuite la pivoter de 90 degres.
        Bitboard temp = cRangeeMask[j][k];
        temp <<= i * 8;

        // La pivoter de 90 degrees.
        Bitboard tempR90R = 0;
        while (temp) {
          int pos = dernierBit(temp);
          tempR90R |= mask[Rotate90RMap[pos]];
          temp ^= mask[pos];
        }
        cb->AttColonne[Rotate90RMap[i * 8 + j]][k] = tempR90R;
      }
    }
  }

  // Maintenant pour les diagonales.
  for (i = 0; i < 64; i++) { // Pour toutes les positions.

    // Commencer par les diagonales dans le sens de A8-H1.
    // Pour toutes les combinaisons possibles.
    for (k = 0; k < (1 << LongueurDiagonaleA8_H1[i]); k++) {

      // Prendre la rangee.
      Bitboard temp = cRangeeMask[PositionDansDiagA8_H1[i]][k];
      temp &= (1 << LongueurDiagonaleA8_H1[i]) - 1;

      // La decaler.
      temp <<= DecalageDiagonaleR[i];

      // La pivoter de 45 degres.
      Bitboard tempR45R = 0;
      while (temp) {
        int pos = dernierBit(temp);
        tempR45R |= mask[Rotate45RMap[pos]];
        temp ^= mask[pos];
      }
      cb->AttDiagonaleA8_H1[i][k] = tempR45R;
    }

    // Maintenant, les diagonales dans le sens de H8-A1.
    // Pour toutes les combinaisons possibles.
    for (k = 0; k < (1 << LongueurDiagonaleH8_A1[i]); k++) {

      // Prendre la rangee.
      Bitboard temp = cRangeeMask[PositionDansDiagH8_A1[i]][k];
      temp &= (1 << LongueurDiagonaleH8_A1[i]) - 1;

      // La decaler.
      temp <<= DecalageDiagonaleL[i];

      // La pivoter de 45 degres.
      Bitboard tempR45L = 0;
      while (temp) {
        int pos = dernierBit(temp);
        tempR45L |= mask[Rotate45LMap[pos]];
        temp ^= mask[pos];
      }
      cb->AttDiagonaleH8_A1[i][k] = tempR45L;
    }
  }

  // Creation des mask pour les pions doubles.
  // Les pions blancs.
  for (i = 55; i >= 16; i--) {
    // Les pions blancs.
    for (j = i - 8; j >= 8; j -= 8)
      cb->PionDoubleBlanc[i] |= mask[j];
  }

  // Les pions noirs.
  for (i = 8; i <= 47; i++) {
    // Les pions blancs.
    for (j = i + 8; j <= 55; j += 8)
      cb->PionDoubleNoir[i] |= mask[j];
  }

  // Masques des colonnes et des rangees.
  for (i = 0; i < 8; i++) {
    Bitboard temp = 0;
    for (j = i; j < 64; j += 8) {
      temp |= mask[j];
    }
    cb->MasqueColonne[i] = temp;
    temp = 0;
    for (j = i * 8; j < (i + 1) * 8; j++) {
      temp |= mask[j];
    }
    cb->MasqueRangee[i] = temp;
  }

  // Les masques pour chaque direction.
  for (i = 0; i < BOARDSIZE; i++) {
    // Direction 1
    for (j = i + 1; j % 8 != 0 && j < 64; j++) {
      cb->plus1dir[i] |= mask[j];
    }

    // Direction 7
    for (j = i + 7; j % 8 != 7 && j < 64; j += 7) {
      cb->plus7dir[i] |= mask[j];
    }

    // Direction 8
    for (j = i + 8; j < 64; j += 8) {
      cb->plus8dir[i] |= mask[j];
    }

    // Direction 9
    for (j = i + 9; j % 8 != 0 && j < 64; j += 9) {
      cb->plus9dir[i] |= mask[j];
    }

    // Direction -1
    for (j = i - 1; j % 8 >= 0 && j >= 0; j--) {
      cb->minus1dir[i] |= mask[j];
    }

    // Direction -7
    for (j = i - 7; j % 8 != 0 && j >= 0; j -= 7) {
      cb->minus7dir[i] |= mask[j];
    }

    // Direction -8
    for (j = i - 8; j >= 0; j -= 8) {
      cb->minus8dir[i] |= mask[j];
    }

    // Direction -9
    for (j = i - 9; j % 8 != 7 && j >= 0; j -= 9) {
      cb->minus9dir[i] |= mask[j];
    }
  }

  for (int source = 0; source < BOARDSIZE; source++) {
    for (int dest = 0; dest < BOARDSIZE; dest++) {
      int dirx = (dest % 8) - (source % 8);
      int diry = (dest / 8) - (source / 8);
      int direction = 0;
      if (diry >= 1) {
        if (dirx >= 1)
          direction = 9;
        else if (dirx == 0)
          direction = 8;
        else if (dirx <= -1)
          direction = 7;
      } else if (diry == 0) {
        if (dirx >= 1)
          direction = 1;
        else if (dirx <= -1)
          direction = -1;
      } else if (diry <= 1) {
        if (dirx >= 1)
          direction = -7;
        else if (dirx == 0)
          direction = -8;
        else if (dirx <= -1)
          direction = -9;
      } // if

      if ((abs(dirx) != abs(diry)) && (dirx != 0 && diry != 0)) {
        direction = 0;
      }
      cb->directions[source][dest] = direction;
    } // for
  } // for

  cb->CaseBlanche = 0xAA55AA55AA55AA55LL;
  cb->CaseNoire = 0x55AA55AA55AA55AALL;
}

void addKiller(TChessBoard *cb, TMove move, int ply) {
  // Score est utilise ici comme un compteur de nombre de fois qu'il
  // a ete utilise.
  if (cb->Killers[ply][0].Score <= cb->Killers[ply][1].Score) {
    move.Score = 0;
    cb->Killers[ply][0] = move;
  } else {
    move.Score = 0;
    cb->Killers[ply][1] = move;
  }
}

// Enleve tout ce qui est en echec.
void choisiMove(TChessBoard *cb, TMoveList *ml, int ply, int wtm) {
#ifdef JOURNAL
  log.log("ChoisiMove: NbMove: %d\n", nbmove);
#endif
  if (ml->nbmove == 0)
    return;

  ml->currmove = 0;
  int iDernierValide = -1;
  // Verifier chaque coup.
  while (ml->currmove < ml->nbmove) {

    // Effectuer le coup et verifier si est en echec.
    makeMove(cb, ply, ml->moves[ml->currmove], wtm);
    int bEstEchec = check(cb, wtm);
    unmakeMove(cb, ply, ml->moves[ml->currmove], wtm);

    // Si le coup est valide le copier dans la case suivant le dernier
    // coup valide trouver.
    if (!bEstEchec) {
      iDernierValide++;
      ml->moves[iDernierValide] = ml->moves[ml->currmove];
    }

    ml->currmove++;
  }
  // Nombre de coup valide.
  ml->nbmove = iDernierValide + 1;
}

// Enleve tout ce qui est en echec.
void choisiNonQuiet(TChessBoard *cb, TMoveList *ml, int ply, int wtm) {
  // Verifier chaque coup.
  for (int i = 0; i < ml->nbmove; i++) {
    ml->moves[i].Score =
        ValeurPiece[ml->moves[i].Capture] - ValeurPiece[ml->moves[i].Piece];
  }
}

void copie(TMoveList *dest, TMoveList *src) {
  memcpy(dest->moves, src->moves, src->nbmove * sizeof(TMove));
}

void tri(TMoveList *ml) {
  if (ml->nbmove <= 0)
    return;
  for (int i = 0; i < ml->nbmove - 1; i++)
    for (int j = i + 1; j < ml->nbmove; j++)
      if (ml->moves[i].Score < ml->moves[j].Score) {
        TMove move = ml->moves[i];
        ml->moves[i] = ml->moves[j];
        ml->moves[j] = move;
      }
}

void ajouteMove(TMoveList *ml, TMove move) { ml->moves[ml->nbmove++] = move; }

void vide(TMoveList *ml) {
  ml->nbmove = 0;
  ml->currmove = 0;
}

TMove currentMove(TMoveList *ml) { return ml->moves[ml->currmove]; }

void setCurrentMoveScore(TMoveList *ml, int score) {
  ml->moves[ml->currmove].Score = score;
}

int movesAreEqual(TMove *dest, TMove *src) {
  return dest->From == src->From && dest->To == src->To;
}

ostream& operator<<(ostream& os, const TChessBoard* cb) {
  for(int i=A8; i<=H1; i++) {
    cout << setw(2) << (int)cb->board[i] << " ";
    if (i % 8 == 7) 
      cout << endl;
  }
  return os;
}

ostream& operator<<(ostream& os, const TMove& move) {
  char text[10];
  sortieMove(move, text);
  cout << text;
  return os;
}

