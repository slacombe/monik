//---------------------------------------------------------------------------
#ifndef UtileH
#define UtileH

#include <math.h>
#include <signal.h>

#include "board.h"
#include "chess.h"
#include "attaque.h"

extern int Rotate90RMap[];
extern int Rotate90LMap[];
extern int Rotate45RMap[];
extern int Rotate45LMap[];
extern int LongueurDiagonaleH8_A1[];
extern int LongueurDiagonaleA8_H1[];
extern int DecalageDiagonaleL[];
extern int DecalageDiagonaleR[];
extern int Rotate45RRealMap[];
extern int Rotate45LRealMap[];
extern int PositionDansDiagA8_H1[];
extern int PositionDansDiagH8_A1[];
extern int PromoteExtension[];

// Les bitboards.
// Masque chaque position.
extern  Bitboard mask[BOARDSIZE];

// Retourne le dernier bit d'un entier de 64 bits.
uint32 dernierBit(Bitboard bitboard);

// Retourne le bitboard d'attaque de la position demande.
Bitboard attaqueRangee(TChessBoard *cb, int i_iPosition);
Bitboard attaqueColonne(TChessBoard *cb, int i_iPosition );
Bitboard attaqueTour(TChessBoard *cb, int i_iPosition);
Bitboard attaqueDame(TChessBoard *cb, int i_iPosition);
Bitboard attaqueFou(TChessBoard *cb, int i_iPosition);

unsigned int Random32();
Bitboard Random64();

// Verifier si le joueur est en echec.
int check(TChessBoard *cb, int wtm);

void swap(TMove& m1, TMove& m2);
int echange(TChessBoard *cb, int source, int dest, int wtm);
Bitboard echangeRayonX(TChessBoard *cb, Bitboard attaque, int source, int direction);

int trouverDernierJournal();

int gagneOpposition(int doit_joue, int roi_blanc, int roi_noir);

// Des macros pour travailler sur les bits.
inline void ClearBit(Bitboard& board, Bitboard position) {
  board &= ~mask[position];
}

inline void ClearR90R(Bitboard& board, Bitboard position) {
  board &= ~mask[Rotate90RMap[ position ]];
}

inline void ClearR90L(Bitboard& board, Bitboard position) {
  board &= ~mask[Rotate90LMap[ position]];
}

inline void ClearR45L(Bitboard& board, Bitboard position) {
  board &= ~mask[Rotate45LRealMap[position]];
}

inline void ClearR45R(Bitboard& board, Bitboard position) {
  board &= ~mask[Rotate45RRealMap[position]];
}

inline void SetBit(Bitboard& board, Bitboard position) {
  board |= mask[position];
}

inline void SetR90R(Bitboard& board, Bitboard position) {
  board |= mask[Rotate90RMap[position]];
}

inline void SetR90L(Bitboard& board, Bitboard position) {
  board |= mask[Rotate90LMap[position]];
}

inline void SetR45L(Bitboard& board, Bitboard position) {
  board |= mask[Rotate45LRealMap[position]];
}

inline void SetR45R(Bitboard& board, Bitboard position) {
  board |= mask[Rotate45RRealMap[position]];
}

inline void MovePiece(Bitboard& board, const TMove& move) {
  ClearBit(board, move.From);
  SetBit(board, move.To);
}

inline void MovePieceR90R(Bitboard& board, const TMove& move) {
  ClearR90R(board, move.From);
  SetR90R(board, move.To);
}

inline void MovePieceR90L(Bitboard& board, const TMove& move) {
  ClearR90L(board, move.From);
  SetR90L(board, move.To);
}

inline void MovePieceR45L(Bitboard& board, const TMove& move) {
  ClearR45L(board, move.From);
  SetR45L(board, move.To);
}

inline void MovePieceR45R(Bitboard& board, const TMove& move) {
  ClearR45R(board, move.From);
  SetR45R(board, move.To);
}

constexpr int CalculX(int pos) {
  return pos % 8;
}

constexpr int CalculY(int pos) {
  return pos / 8;
}

constexpr int Carre(int valeur) {
  return valeur * valeur;
}

constexpr int DistanceX(int v1, int v2) {
  return abs(CalculX(v2) - CalculX(v1));
}

constexpr int DistanceY(int v1, int v2) {
  return abs(CalculY(v2) - CalculY(v1));
}

constexpr int File(int pos) {
  return pos & 0x07;
}

constexpr int Rank(int pos) {
  return (pos >> 3) & 0x07;
}

constexpr int FileDistance(int a, int b) {
  return abs((a & 7) - (b & 7));
}

constexpr int RankDistance(int a, int b) {
  return abs((a >> 3) - (b >> 3));
}

constexpr int min(int x, int y) {
  return (x > y) ? y : x;
}

// En colonne et en rangee.
constexpr int Distance(int pos1, int pos2) {
  return DistanceX(pos1, pos2) + DistanceY(pos1, pos2);
}

// Macro qui determine si c'est le temp d'appeler quiescence ou
// encore Search.
constexpr auto ABSearch = [](auto* cb, int depth, int ply, int wtm, int alpha, int beta) {
  return (depth <= 0 ? quiescence(cb, ply, wtm, alpha, beta) : search(cb, depth, ply, wtm, alpha, beta));
};

// Macro qui OR les bitboars.
constexpr Bitboard Cavalier(TChessBoard *cb) { return (cb->cavalierb | cb->cavaliern); }
constexpr Bitboard Fou(TChessBoard *cb) { return (cb->foub | cb->foun); }
constexpr Bitboard Tour(TChessBoard *cb) { return (cb->tourb | cb->tourn); }
constexpr Bitboard Dame(TChessBoard *cb) { return (cb->dameb | cb->damen); }
constexpr Bitboard DameFou(TChessBoard *cb) { return (Dame(cb) | Fou(cb)); }
constexpr Bitboard DameTour(TChessBoard *cb) { return (Dame(cb) | Tour(cb)); }

constexpr Bitboard And(Bitboard x, Bitboard y) { return (x & y); }
constexpr Bitboard Or(Bitboard x, Bitboard y) { return (x | y); }
constexpr Bitboard Xor(Bitboard x, Bitboard y) { return (x ^ y); }

constexpr int FILEA = 0;
constexpr int FILEB = 1;
constexpr int FILEC = 2;
constexpr int FILED = 3;
constexpr int FILEE = 4;
constexpr int FILEF = 5;
constexpr int FILEG = 6;
constexpr int FILEH = 7;

constexpr int RANK8 = 0;
constexpr int RANK7 = 1;
constexpr int RANK6 = 2;
constexpr int RANK5 = 3;
constexpr int RANK4 = 4;
constexpr int RANK3 = 5;
constexpr int RANK2 = 6;
constexpr int RANK1 = 7;

constexpr int RoqueBlancDame(TChessBoard *cb, int ply) { return (cb->Roque & ROQUEDAMEBLANC); }
constexpr int RoqueBlancRoi(TChessBoard *cb, int ply) { return (cb->Roque & ROQUEROIBLANC); }
constexpr int RoqueNoirDame(TChessBoard *cb, int ply) { return (cb->Roque & ROQUEDAMENOIR); }
constexpr int RoqueNoirRoi(TChessBoard *cb, int ply) { return (cb->Roque & ROQUEROINOIR); }

constexpr bool RoqueBlanc(TChessBoard *cb, int ply) { return And(RoqueBlancDame(cb, ply), RoqueBlancRoi(cb, ply)); }
constexpr bool RoqueNoir(TChessBoard *cb, int ply) { return And(RoqueNoirDame(cb, ply), RoqueNoirRoi(cb, ply)); }

// Verifie si le joueur est en echec.
inline int check(TChessBoard *cb, int wtm)
{
  if ( wtm ) {
    return attacked(cb, cb->PositionRoiBlanc, !wtm);
  }
  else {
    return attacked(cb, cb->PositionRoiNoir, !wtm);
  }
}

//---------------------------------------------------------------------------
#endif
