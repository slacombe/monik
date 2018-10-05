//-----------------------------------------------------------------------------
// Projet: Monik.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Board.h
// Debut: 2 novembre 1998.
//
//---------------------------------------------------------------------------
#ifndef BoardH
#define BoardH

#include "chess.h"

extern TMove pv[MAXPLY][MAXPLY];
extern int pv_length[MAXPLY];

// Les positions.
class TChessBoard
{
public:
  int NoCoups;
  TMove ListeMove[MAXMOVE];
  int ListeRoque[MAXMOVE];

  // Regle du 50 coup sans changement.
  int Regle50Coup;
  int Regle50CoupStore[MAXMOVE];

  Bitboard CleHachage;

  Bitboard Positions[MAXMOVE];

  // Etat des pieces pour le roque.
  // bit 1 = piece bouge.
  // Bit poid fort = noir.
  // Bit poid faible = blanc.
  // Bit 2 et 5 = tour dame.
  // Bit 1 et 4 = tour roi.
  // bit 3 et 0 = roi.
  char Roque;

  // Le materiel.
  int NbWhitePieces;
  int TotalPionBlanc;
  int TotalCavalierBlanc;
  int TotalFouBlanc;
  int TotalTourBlanche;
  int TotalDameBlanche;
  int TotalMaterielBlanc;
  int NbBlackPieces;
  int TotalPionNoir;
  int TotalCavalierNoir;
  int TotalFouNoir;
  int TotalTourNoire;
  int TotalDameNoire;
  int TotalMaterielNoir;

  int PositionRoiBlanc;
  int PositionRoiNoir;

  // Valeur positionnelle.
  SCORE ScoreMaterielBlanc;
  SCORE ScorePositionBlanc;
  SCORE ScoreMaterielNoir;
  SCORE ScorePositionNoir;

  SCORE ScoreTourBlanche;
  SCORE ScoreTourNoir;


  // Case de la possibilite d'un en passant.
  int EnPassantB[MAXPLY], EnPassantN[MAXPLY];

  // Le board.
  Piece board[BOARDSIZE];

  // Toute les pieces presentes.
  Bitboard piece;
  Bitboard pieceR90R;
  Bitboard pieceR90L;
  Bitboard pieceR45L;
  Bitboard pieceR45R;

  // Toute les pieces blanches et noires.
  Bitboard pieceb;
  Bitboard piecen;

  // Les cases vides.
  Bitboard vide;

  // Les pions.
  Bitboard pionb;
  Bitboard pionn;
  Bitboard pionbdepart;
  Bitboard pionndepart;

  // Les cavaliers.
  Bitboard cavalierb;
  Bitboard cavaliern;

  // Les fous.
  Bitboard foub;
  Bitboard foun;

  // Les tours.
  Bitboard tourb;
  Bitboard tourn;

  // Les dames.
  Bitboard dameb;
  Bitboard damen;

  // Les rois.
  Bitboard roib;
  Bitboard roin;

  // Les attaques de chaque pieces qui ne glissent pas.
  Bitboard AttaquePionBlanc[BOARDSIZE];
  Bitboard AttaquePionNoir[BOARDSIZE];
  Bitboard AttaqueCavalier[BOARDSIZE];
  Bitboard AttaqueRoi[BOARDSIZE];

  // Les attaques des pieces qui glissent.
  Bitboard AttRangee[BOARDSIZE][256];
  Bitboard AttColonne[BOARDSIZE][256];
  Bitboard AttDiagonaleA8_H1[BOARDSIZE][256];
  Bitboard AttDiagonaleH8_A1[BOARDSIZE][256];

  Bitboard PionDoubleBlanc[BOARDSIZE];
  Bitboard PionDoubleNoir[BOARDSIZE];

  Bitboard MasqueColonne[8];
  Bitboard MasqueRangee[8];
  int PionPasseB[8];
  int PionPasseN[8];

  // Les masques pour chaque directions.
  Bitboard plus1dir[BOARDSIZE];
  Bitboard plus7dir[BOARDSIZE];
  Bitboard plus8dir[BOARDSIZE];
  Bitboard plus9dir[BOARDSIZE];
  Bitboard minus1dir[BOARDSIZE];
  Bitboard minus7dir[BOARDSIZE];
  Bitboard minus8dir[BOARDSIZE];
  Bitboard minus9dir[BOARDSIZE];

  Bitboard CaseBlanche;
  Bitboard CaseNoire;

  int OutOfBook;

  // Les directions.
  int directions[BOARDSIZE][BOARDSIZE];

  // On donne un penalite pour les pieces qui n'ont pas
  // encore bouge.
  int Deploiement[BOARDSIZE];

  // La liste des coups.
  TMoveList MoveList[MAXPLY];

  TMoveList CurrentPath;

  TMove HashMove[MAXPLY];

  int RaisonExtension[MAXPLY];

  bool inCheck[MAXPLY];
  int m_iNbExtEchec;

  // 2 Killer move.
  TMove Killers[MAXPLY][2];
  void AddKiller( TMove* move, int ply );

  TChessBoard();  // Constructeur.
  void InitialiseBoard();
  void InitialiseBitboard();
};

extern TChessBoard cb;

//---------------------------------------------------------------------------
#endif
