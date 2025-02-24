//---------------------------------------------------------------------------
#ifndef TranspositionH
#define TranspositionH

#include "chess.h"
#include "board.h"

// Signification des bit dans une entree de transposition.
//  Bits      Type      Pos      Description
//  2         Type      62       0 = Invalide. 1 = Score exacte,
//                               2 = Borne inferieur, 3 = Borne superieur.
//  1         Danger    60       1 = Etendre ce sous-arbre.
//  16        Valeur    59       Valeur ou bound.
//  32        Coup      43       Un coup a chercher.
//  12        Depth     11       Profondeur ou le coup est valide.

#ifdef TRANSPOSITION

extern int g_transpositionHit;
extern int g_transpositionWrite;
extern int g_transpositionOverwrite;
extern int g_transpositionRefutation;

#define HASH_MISS   0
#define EXACT_SCORE 1
#define UPPER_BOUND 2
#define LOWER_BOUND 3

constexpr uint64 multiplier_meg = 1024 * 1024;
constexpr uint64 multiplier_gig = 1024 * 1024 * 1024;

struct hash_entry {
  Bitboard key;
  int score: 16;
  unsigned int entry_type: 2;
  unsigned int depth: 6;
  unsigned int danger: 1;
  unsigned int from: 6;
  unsigned int to: 6;
  unsigned int piece: 4;
  unsigned int capture: 4;
  unsigned int roque: 1;
  unsigned int en_passant: 1;
  unsigned int promotion: 4;
  unsigned int pad: 4;
};

bool transpositionTableCreated();
void createTranspositionTable(uint64 size);
void createTranspositionTable(std::string size);
void freeTranspositionTable();

void displayTranspositionStats();
void clearStats();

// Chercher une position dans la table.
uint32 lookup(TChessBoard *cb, int ply, int depth, int *alpha, int *beta, int *danger);

uint32 storeRefutation(TChessBoard *cb, int ply, int depth, short valeur, int danger);

uint32 storeBest(TChessBoard *cb, int ply, int depth, int alpha, int initial_alpha, int danger);

#endif

#endif
