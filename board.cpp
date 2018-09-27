//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Board.cpp
// Debut: 2 novembre 1998.
//
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "init.h"
#include "chess.h"
#include "utile.h"
#include "board.h"
#include "eval.h"

// Quelques variables globales.
TChessBoard cb;

TMove pv[MAXPLY][MAXPLY];
int pv_length[MAXPLY];

TChessBoard::TChessBoard()
{
  InitialiseData();

  // Initialise le board.
  InitialiseBoard();

  // Initialise les bitboards.
  InitialiseBitboard();
}

void TChessBoard::InitialiseBoard()
{
  // On met toute notre structure a zero.
  memset( this, 0, sizeof( TChessBoard ) );

  // Les pions.
  int i;
  for( i=A7; i<=H7; i++ )
    board[i] = -pion; // Noirs.
  for( i=A2; i<=H2; i++ )
    board[i] = pion;  // Blancs.

  // Les cavaliers.
  board[B1] = cavalier;
  board[G1] = cavalier;
  board[B8] = -cavalier;
  board[G8] = -cavalier;

  // Les fous.
  board[C1] = fou;
  board[F1] = fou;
  board[C8] = -fou;
  board[F8] = -fou;

  // Les tours.
  board[A1] = tour;
  board[H1] = tour;
  board[A8] = -tour;
  board[H8] = -tour;

  // Les dames.
  board[D1] = dame;
  board[D8] = -dame;

  // Les rois.
  board[E1] = roi;
  board[E8] = -roi;

  PositionRoiBlanc = E1;
  PositionRoiNoir  = E8;

  for( i=0; i<MAXPLY; i++ ) {
    EnPassantN[i] = -1;
    EnPassantB[i] = -1;
  }

  NoCoups = 0;
}

void TChessBoard::InitialiseBitboard()
{
  // On initialise la table des masks.
  // On utilise normalement cette table en combinaison avec une autre pour
  // determiner certaine condition.
  Bitboard b = 1;
  int i, j, k;
  for( i=A8; i<=H1; i++ )
    mask[i] = (b << i);

  // Pour chaque piece sur le jeu on initialise ses bitboards.
  for( i=A8; i<=H1; i++ ) {
    switch( board[i] ) {
      // Pion blanc.
      case pion:
        pionb |= mask[i];
        TotalPionBlanc++;
        TotalMaterielBlanc++;
        ScoreMaterielBlanc += VALPION;
        CleHachage ^= ClePionB[i];
        break;
      case -pion:
        pionn |= mask[i];
        TotalPionNoir++;
        TotalMaterielNoir++;
        ScoreMaterielNoir += VALPION;
        CleHachage ^= ClePionN[i];
        break;
      case cavalier:
        cavalierb |= mask[i];
        TotalCavalierBlanc++;
        TotalMaterielBlanc++;
        NbWhitePieces++;
        ScoreMaterielBlanc += VALCAVALIER;
        CleHachage ^= CleCavalierB[i];
        break;
      case -cavalier:
        cavaliern |= mask[i];
        TotalCavalierNoir++;
        TotalMaterielNoir++;
        NbBlackPieces++;
        ScoreMaterielNoir += VALCAVALIER;
        CleHachage ^= CleCavalierN[i];
        break;
      case fou:
        foub |= mask[i];
        TotalFouBlanc++;
        TotalMaterielBlanc++;
        NbWhitePieces++;
        ScoreMaterielBlanc += VALFOU;
        CleHachage ^= CleFouB[i];
        break;
      case -fou:
        foun |= mask[i];
        TotalFouNoir++;
        TotalMaterielNoir++;
        NbBlackPieces++;
        ScoreMaterielNoir += VALFOU;
        CleHachage ^= CleFouN[i];
        break;
      case tour:
        tourb |= mask[i];
        TotalTourBlanche++;
        TotalMaterielBlanc++;
        NbWhitePieces++;
        ScoreMaterielBlanc += VALTOUR;
        CleHachage ^= CleTourB[i];
        break;
      case -tour:
        tourn |= mask[i];
        TotalTourNoire++;
        TotalMaterielNoir++;
        NbBlackPieces++;
        ScoreMaterielNoir += VALTOUR;
        CleHachage ^= CleTourN[i];
        break;
      case dame:
        dameb |= mask[i];
        TotalDameBlanche++;
        TotalMaterielBlanc++;
        NbWhitePieces++;
        ScoreMaterielBlanc += VALDAME;
        CleHachage ^= CleDameB[i];
        break;
      case -dame:
        damen |= mask[i];
        TotalDameNoire++;
        TotalMaterielNoir++;
        NbBlackPieces++;
        ScoreMaterielNoir += VALDAME;
        CleHachage ^= CleDameN[i];
        break;
      case roi:
        roib |= mask[i];
        PositionRoiBlanc = i;
        CleHachage ^= CleRoiB[i];
        break;
      case -roi:
        roin |= mask[i];
        PositionRoiNoir = i;
        CleHachage ^= CleRoiN[i];
        break;
    }
    if ( board[i] ) {
      piece |= mask[i];
      pieceR90L |= mask[Rotate90LMap[i]];
      pieceR45L |= mask[Rotate45LRealMap[i]];
      pieceR45R |= mask[Rotate45RRealMap[i]];

      if ( board[i] > 0 ) {
        pieceb |= mask[i];
      }
      else {
        piecen |= mask[i];
      }
    }

  } // for

  // Les cases vides.
  vide = ~piece;

  // Les masks de departs des pions. Pour voir si un pion etait a sa case
  // depart afin de pouvoir genere l'avancement de 2 cases.
  pionndepart =  0x0000000000FF0000LL;
  pionbdepart =  0x0000FF0000000000LL;

  // Les attaques possibles pour chaque pieces.
  // Les pions blancs.
  for( i=A7; i<=H1; i++ ) {
    Bitboard piecebd = 0;
    if ( i % 8 != 0 )
      piecebd = mask[i-9];
    if ( i % 8 != 7 )
      piecebd |= mask[i-7];
    AttaquePionBlanc[i] = piecebd;
  }

  // Les pions noirs.
  for( i=A8; i<=H2; i++ ) {
    Bitboard piecebd = 0;
    if ( i % 8 != 0 )
      piecebd = mask[i+7];
    if ( i % 8 != 7 )
      piecebd |= mask[i+9];
    AttaquePionNoir[i] = piecebd;
  }
  // Les cavaliers.
  int moveCavalier[] = { 17, 15, 10, 6, -6, -10, -15, -17 };
  for( i=A8; i<=H1; i++ ) {
    int iRangeeS = i/8;
    int iColonneS = i&7;
    for( j=0; j<8; j++ ) {
      int iDest = i+moveCavalier[j];
      if ( iDest < 0 || iDest > 63 )
        continue;
      int iRangeeD = iDest/8;
      int iColonneD = iDest&7;
      if ( abs( iRangeeS-iRangeeD ) > 2 ||
           abs( iColonneS-iColonneD ) > 2 )
        continue;
      AttaqueCavalier[i] |= mask[i+moveCavalier[j]];
    }
  }

  // Les bitboards d'attaque du roi.
  int moveRoi[] = { 9, 8, 7, 1, -1, -7, -8, -9 };
  for( i=A8; i<=H1; i++ ) {
    int iRangeeS = i/8;
    int iColonneS = i&7;
    for( int j=0; j<8; j++ ) {
      int iDest = i+moveRoi[j];
      if ( iDest < 0 || iDest > 63 )
        continue;
      int iRangeeD = iDest/8;
      int iColonneD = iDest&7;
      if ( abs( iRangeeS-iRangeeD ) > 1 ||
           abs( iColonneS-iColonneD ) > 1 )
        continue;
      AttaqueRoi[i] |= mask[i+moveRoi[j]];
    }
  }

  // Les bitboards d'attaque des rangee et colonnes.
  // Pour les dames et les tours.
  unsigned char cMaskPosR = 0x01,
                cRangeeMask[8][256];
  for( i=0; i<8; i++ ) { // Pour chaque position sur la rangee.
    for( int j=0; j<256; j++ ) { // Pour toute les combinaisons possibles.
      // On decale le bit vers la droite jusqu'a ce qu'on rencontre un bit
      // ou la fin.
      unsigned char cTempMaskR = cMaskPosR,
                    cMaskAttR = 0;
      do {
        cTempMaskR >>= 1;
        cMaskAttR |= cTempMaskR;
      } while( !(cTempMaskR & j) && cTempMaskR != 0 );

      // La meme chose vers la gauche.
      cTempMaskR = cMaskPosR;
      do {
        cTempMaskR <<= 1;
        cMaskAttR |= cTempMaskR;
      } while( !(cTempMaskR & j) && (cTempMaskR & 0xFF) != 0 );

      // On le place dans l'index.
      cRangeeMask[i][j] = cMaskAttR;

    }

    // Prochaine position.
    cMaskPosR <<= 1;
  }

  // Maintenant, on prend les rangees et on les inseres dans les
  // bitboards d'attaque de rangee.
  for( i=0; i<8; i++ ) { // Pour les 8 rangees du board.
    for( j=0; j<8; j++ ) { // Pour chaque position dans la rangee.
      for( k=0; k<256; k++ ) {
        Bitboard temp = cRangeeMask[j][k];
        temp <<= i*8;
        AttRangee[i*8+j][k] = temp;
      }
    }
  }

  // Maintenant, prendre les rangees et on les inseres dans les
  // bitboards d'attaque de colonne.
  for( i=0; i<8; i++ ) { // Pour le 8 colonnes du board.
    for( j=0; j<8; j++ ) { // Pour chaque position dans la colonne.
      for( k=0; k<256; k++ ) {
        // Ici, prendre la rangee, la mettre dans le bitboard, la decaler
        // et ensuite la pivoter de 90 degres.
        Bitboard temp = cRangeeMask[j][k];
        temp <<= i*8;

        // La pivoter de 90 degrees.
        Bitboard tempR90R = 0;
        while( temp ) {
          int pos = DernierBit( temp );
          tempR90R |= mask[Rotate90RMap[pos]];
          temp ^= mask[pos];
        }
        AttColonne[ Rotate90RMap[ i*8+j ] ][k] = tempR90R;
      }
    }
  }

  // Maintenant pour les diagonales.
  for( i=0; i<64; i++ ) { // Pour toutes les positions.

    // Commencer par les diagonales dans le sens de A8-H1.
    // Pour toutes les combinaisons possibles.
    for( k=0; k<(1 << LongueurDiagonaleA8_H1[i]); k++ ) {

      // Prendre la rangee.
      Bitboard temp = cRangeeMask[PositionDansDiagA8_H1[i]][k];
      temp &= (1 << LongueurDiagonaleA8_H1[i])-1;

      // La decaler.
      temp <<= DecalageDiagonaleR[i];

      // La pivoter de 45 degres.
      Bitboard tempR45R = 0;
      while( temp ) {
        int pos = DernierBit( temp );
        tempR45R |= mask[Rotate45RMap[pos]];
        temp ^= mask[pos];
      }
      AttDiagonaleA8_H1[i][k] = tempR45R;
    }

    // Maintenant, les diagonales dans le sens de H8-A1.
    // Pour toutes les combinaisons possibles.
    for( k=0; k<(1 << LongueurDiagonaleH8_A1[i]); k++ ) {

      // Prendre la rangee.
      Bitboard temp = cRangeeMask[PositionDansDiagH8_A1[i]][k];
      temp &= (1 << LongueurDiagonaleH8_A1[i])-1;

      // La decaler.
      temp <<= DecalageDiagonaleL[i];

      // La pivoter de 45 degres.
      Bitboard tempR45L = 0;
      while( temp ) {
        int pos = DernierBit( temp );
        tempR45L |= mask[Rotate45LMap[pos]];
        temp ^= mask[pos];
      }
      AttDiagonaleH8_A1[i][k] = tempR45L;
    }
  }

  // Creation des mask pour les pions doubles.
  // Les pions blancs.
  for( i=55; i>=16; i-- ) {
    // Les pions blancs.
    for( j=i-8; j>=8; j-=8 )
      PionDoubleBlanc[i] |= mask[j];
  }

  // Les pions noirs.
  for( i=8; i<=47; i++ ) {
    // Les pions blancs.
    for( j=i+8; j<=55; j+=8 )
      PionDoubleNoir[i] |= mask[j];
  }

  // Masques des colonnes et des rangees.
  for( i=0; i<8; i++ ) {
    Bitboard temp = 0;
    for( j=i; j<64; j+=8 )
      temp |= mask[j];
    MasqueColonne[i] = temp;
    temp = 0;
    for( j=i*8; j<(i+1)*8; j++ )
      temp |= mask[j];
    MasqueRangee[i] = temp;
  }

  // Les masques pour chaque direction.
  for( i=0; i<BOARDSIZE; i++ ) {
    // Direction 1
    for( j=i+1; j%8!=0 && j<64; j++ )
      plus1dir[i] |= mask[j];

    // Direction 7
    for( j=i+7; j%8!=7 && j<64; j+=7 )
      plus7dir[i] |= mask[j];

    // Direction 8
    for( j=i+8; j<64; j+=8 )
      plus8dir[i] |= mask[j];

    // Direction 9
    for( j=i+9; j%8!=0 && j<64; j+=9 )
      plus9dir[i] |= mask[j];

    // Direction -1
    for( j=i-1; j%8>=0 && j>=0; j-- )
      minus1dir[i] |= mask[j];

    // Direction -7
    for( j=i-7; j%8!=0 && j>=0; j-=7 )
      minus7dir[i] |= mask[j];

    // Direction -8
    for( j=i-8; j>=0; j-=8 )
      minus8dir[i] |= mask[j];

    // Direction -9
    for( j=i-9; j%8!=7 && j>=0; j-=9 )
      minus9dir[i] |= mask[j];
  }

  for( int source=0; source<BOARDSIZE; source++ ) {
    for( int dest=0; dest<BOARDSIZE; dest++ ) {
      int dirx = (dest%8)-(source%8);
      int diry = (dest/8)-(source/8);
      int direction = 0;
      if ( diry >= 1 ) {
        if ( dirx >= 1 )
          direction = 9;
        else if ( dirx == 0 )
          direction = 8;
        else if ( dirx <= -1 )
          direction = 7;
      }
      else if ( diry == 0 ) {
        if ( dirx >= 1 )
          direction = 1;
        else if ( dirx <= -1 )
          direction = -1;
      }
      else if ( diry <= 1 ) {
        if ( dirx >= 1 )
          direction = -7;
        else if ( dirx == 0 )
          direction = -8;
        else if ( dirx <= -1 )
          direction = -9;
      } // if

      if ( (abs(dirx) != abs(diry)) && (dirx !=0 && diry != 0))
        direction = 0;
      directions[source][dest] = direction;
    } // for
  } // for

  CaseBlanche = 0xAA55AA55AA55AA55LL;
  CaseNoire =   0x55AA55AA55AA55AALL;
}

void TChessBoard::AddKiller( TMove* i_pMove, int ply )
{
  // Score est utilise ici comme un compteur de nombre de fois qu'il
  // a ete utilise.
  if ( Killers[ply][0].Score <= Killers[ply][1].Score ) {
    i_pMove->Score = 0;
    Killers[ply][0] = *i_pMove;
  }
  else {
    i_pMove->Score = 0;
    Killers[ply][1] = *i_pMove;
  }
}
