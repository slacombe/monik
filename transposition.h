//---------------------------------------------------------------------------
#ifndef TranspositionH
#define TranspositionH

#include "chess.h"
#include "board.h"

// Signification des bit dans une entree de transposition.
//  Bits      Type      Pos      Description
//  1         Age       63       1 = on peut remplacer cette entree.
//  2         Type      62       0 = Invalide. 1 = Score exacte,
//                               2 = Borne inferieur, 3 = Borne superieur.
//  1         Danger    60       1 = Etendre ce sous-arbre.
//  16        Valeur    59       Valeur ou bound.
//  32        Coup      43       Un coup a chercher.
//  12        Depth     11       Profondeur ou le coup est valide.
//  64        Cle       63       Cle de validite de la position.

#ifdef TRANSPOSITION

extern int g_transpositionHit;
extern int g_transpositionWrite;
extern int g_transpositionOverwrite;
extern int g_transpositionRefutation;

#define   SCORE_EXACTE      1
#define   BORNE_SUPERIEUR   2
#define   BORNE_INFERIEUR   3
#define   EVITER_NULL       4

#define maskAge      0x8000000000000000LL
#define maskType     0x6000000000000000LL
#define maskDanger   0x1000000000000000LL
#define maskValeur   0x0FFFF00000000000LL
#define maskCoup     0x00000FFFFFFFF000LL
#define Depth        0x0000000000000FFFLL


#define SetAge(x) \
  (x |= maskAge)
#define ClearAge(x) \
  (x &= (~maskAge))
#define GetAge(x) \
  (x & maskAge)
#define StoreType(x, t) \
  (x |= ((Bitboard)(t & 3) << 61))
#define GetType(x) \
  (((Bitboard)x >> 61) & 0x03)
#define StoreDanger(x, d) \
  (x |= ((Bitboard)(d & 1) << 60))
#define GetDanger(x) \
  ((x & maskDanger) >> 60)
#define StoreValeur(x, v) \
  (x |= ((Bitboard)((short)(v) & 0xFFFF) << 44))
#define GetValeur(x) \
  (((short)(x >> 44) & 0xFFFF))
#define StoreCoup(x, c) \
  (x |= ((Bitboard)((c) & 0xFFFFFFFF) << 12))
#define GetCoup(x) \
  ((x & maskCoup) >> 12)
#define StoreDepth(x, d) \
  (x |= ((Bitboard)(d & 0xFFF)))
#define GetDepth(x) \
  (x & 0xFFF)

bool transpositionTableCreated();
void createTranspositionTable(uint32 sizeInMeg);
void freeTranspositionTable();

void initializeTranspositionTable();

void displayTranspositionStats();
void clearStats();

// Chercher une position dans la table.
uint32 lookup(TChessBoard *cb, int ply, int depth,
              int wtm, int *alpha, int *beta, int *danger);

uint32 storeRefutation(TChessBoard *cb, uint32 ply, uint32 depth, uint32 wtm,
                      short valeur, uint32 alpha, uint32 beta, uint32 danger);

uint32 storeBest(TChessBoard *cb, uint32 ply, uint32 depth, uint32 wtm, 
                      uint32 alpha, uint32 initial_alpha, uint32 danger);

#endif

//---------------------------------------------------------------------------
#endif
