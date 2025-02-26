#ifdef TRANSPOSITION

#include <iostream>

#include "board.h"
#include "transposition.h"
#include "make.h"
#include "unmake.h"

using namespace std;

int g_transpositionHit;
int g_transpositionWrite;
int g_transpositionOverwrite;
int g_transpositionRefutation;

uint64 nb_entree;
uint64 mask_cle;

hash_entry *hash_table = 0;

void createTranspositionTable(uint64 size_in_bytes)
{
  if (hash_table) {
    free(hash_table);
  }

  uint64 pos_in_table = size_in_bytes / sizeof(hash_entry);

  // Reconvertir en mega-octets.
  uint64 nb_entree_base2 = 1;
  while (pos_in_table)
  {
    pos_in_table >>= 1;
    nb_entree_base2 <<= 1;
  }
  nb_entree_base2 >>= 1;

  // Alouer la memoire necessaire.
  hash_table = (hash_entry *)calloc(nb_entree_base2, sizeof(hash_entry));

  mask_cle = nb_entree_base2 - 1;

  cout << "Transposition: " << nb_entree_base2 << " positions" << endl;
  cout << "Hash table size: " << (nb_entree_base2 * sizeof(hash_entry) / (1024 * 1024)) << "m" << endl;
  cout << "Mask: 0x" << hex << mask_cle << dec << endl;
}

void createTranspositionTable(std::string size) {
  int l = size.length();
  if (!isalpha(size[l-1])) {
    size += 'm';
  }
  l = size.length();
  char unit = size[l-1];
  string units = size.substr(0, l - 1);
  uint64 multiplier;
  switch(unit)  {
    case 'm':
    case 'M':
      multiplier = multiplier_meg;
      break;
    case 'g':
    case 'G':
      multiplier = multiplier_gig;
      break;
    default:
      cout << "Invalid unit: " << unit << endl;
      cout << "Using megabytes by default" << endl;
      multiplier = multiplier_meg;
  }

  createTranspositionTable(stoi(units) * multiplier);
}

void freeTranspositionTable()
{
  if (hash_table)
  {
    free(hash_table);
    hash_table = 0;
  }
}

bool transpositionTableCreated()
{
  return hash_table;
}

void displayTranspositionStats() {
  printf("Transposition table stats:\n");
  printf("Hit: %d\n", g_transpositionHit);
  printf("Overwrite: %d\n", g_transpositionOverwrite);
  printf("Write: %d\n", g_transpositionWrite);
  printf("Refutation: %d\n", g_transpositionRefutation);
}

void clearStats() {
  g_transpositionWrite = 0;
  g_transpositionRefutation = 0;
}

inline hash_entry* calculate_position(TChessBoard *cb) {
  uint32 position = (mask_cle & (int)cb->CleHachage);
  hash_entry* entry = hash_table +  position;
  return entry;
}

uint32 lookup(TChessBoard *cb, int ply, int depth, int *alpha, int *beta, int *danger)
{
  short valeur;

  // Retrouver la position dans la table.
  hash_entry* pPosition = calculate_position(cb);

  // Est-ce la bonne position?
  if (pPosition->key ^ cb->CleHachage)
  {
    return HASH_MISS;
  }

  if (pPosition->depth < depth)
  {
    return HASH_MISS;
  }
  
  int entry_type = pPosition->entry_type;

  *danger = pPosition->danger;
  valeur = pPosition->score;

  TMove move;
  move.From = pPosition->from;
  move.To = pPosition->to;
  move.Piece = pPosition->piece;
  move.Capture = pPosition->capture;
  move.EnPassant = pPosition->en_passant;
  move.Promotion = pPosition->promotion;
  move.Roque = pPosition->roque;
  move.Score = valeur;
  cb->HashMove[ply] = move;
  g_transpositionHit++;
  switch (entry_type)
  {
  case EXACT_SCORE:
    if (abs(valeur) >= MATE - 100)
    {
      if (pPosition->score > 0)
        valeur -= (ply - 1);
      else
        valeur += (ply - 1);
    }
    move.Score = valeur;
    *alpha = valeur;
    return EXACT_SCORE;
  case UPPER_BOUND:
    if (valeur <= *alpha)
    {
      *alpha = valeur;
      return UPPER_BOUND;
    }
    return HASH_MISS;
    break;
  case LOWER_BOUND:
    if (valeur >= *beta)
    {
      *beta = valeur;
      return LOWER_BOUND;
    }
    break;
  }
  return HASH_MISS;
}

// On utilise cette fonction lorsque un coup refute le coup de la couche
// precedente.
uint32 storeRefutation(TChessBoard *cb, int ply, int depth, short valeur, int danger)
{
  // Retrouver la position dans la table.
  hash_entry* pPosition = calculate_position(cb);

  if (pPosition->key)
  {
    g_transpositionOverwrite++;
  } else {
    g_transpositionWrite++;
  }

  // Mettre les informations dans la table.
  pPosition->key = cb->CleHachage;
  pPosition->score = valeur;
  pPosition->entry_type = LOWER_BOUND;
  pPosition->danger = danger;
  TMove move = pv[ply][ply];
  pPosition->from = move.From;
  pPosition->to = move.To;
  pPosition->piece = move.Piece;
  pPosition->capture = move.Capture;
  pPosition->en_passant = move.EnPassant;
  pPosition->promotion = move.Promotion;
  pPosition->depth = depth;

  return true;
}

// StoreBest est appelle quand tout les noeud d'un coup a ete explore
// et qu'il est temps de renvoyer la valeur du meilleur coup au noeud parent.
uint32 storeBest(TChessBoard *cb, int ply, int depth, int alpha, int initial_alpha, int danger) {
  // Retrouver la position dans la table.
  hash_entry* pPosition = calculate_position(cb);

  if (pPosition->key)
  {
    g_transpositionOverwrite++;
  } else {
    g_transpositionWrite++;
  }

  // Mettre les informations dans la table.
  pPosition->key = cb->CleHachage;
  pPosition->depth = depth;
  int entry_type;
  TMove move = pv[ply][ply];
  if (alpha > initial_alpha)
  {
    pPosition->from = move.From;
    pPosition->to = move.To;
    pPosition->piece = move.Piece;
    pPosition->capture = move.Capture;
    entry_type = EXACT_SCORE;
  }
  else
  {
    pPosition->from = 0;
    pPosition->to = 0;
    pPosition->piece = 0;
    pPosition->capture = 0;
    entry_type = UPPER_BOUND;
  }
  pPosition->score = alpha;
  pPosition->entry_type = entry_type;

  return true;
}

void savePV(TChessBoard *cb, int side, int depth) {
  int wtm = side;
  for(int i=1; i<pv_length[1]; i++) {
    makeMove(cb, i, pv[1][i], wtm);
    storeBest(cb, i, depth, pv[1][i].Score, pv[1][i].Score-1, 0);
    wtm = -wtm;
  }  
  for(int i=pv_length[1]-1; i>=1; i--) {
    wtm = -wtm;
    unmakeMove(cb, i, pv[1][i], wtm);
  }
}

#endif
