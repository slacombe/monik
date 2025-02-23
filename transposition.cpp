#ifdef TRANSPOSITION

#include <iostream>

#include "board.h"
#include "transposition.h"

using namespace std;

int g_transpositionHit;
int g_transpositionWrite;
int g_transpositionOverwrite;
int g_transpositionRefutation;

uint64 nb_entree;
uint64 mask_cle;

hash_entry *white_hash_table = 0;
hash_entry *black_hash_table = 0;

constexpr uint64 multiplier_meg = 1024 * 1024;
constexpr uint64 multiplier_gig = 1024 * 1024 * 1024;

void createTranspositionTable(uint64 size_in_bytes)
{
  uint64 pos_in_table = size_in_bytes / (sizeof(Bitboard) * 2);

  // Reconvertir en mega-octets.
  uint64 nb_entree_base2 = 1;
  while (pos_in_table)
  {
    pos_in_table >>= 1;
    nb_entree_base2 <<= 1;
  }
  // Une de trop et diviser en deux, une moitie pour chaque table.
  nb_entree_base2 >>= 2;

  // Alouer la memoire necessaire.
  white_hash_table = (hash_entry *)calloc(nb_entree_base2, sizeof(hash_entry));
  black_hash_table = (hash_entry *)calloc(nb_entree_base2, sizeof(hash_entry));

  nb_entree = nb_entree_base2;
  mask_cle = nb_entree - 1;

  cout << "Transposition: " << nb_entree * 2 << " positions" << endl;
  cout << "Hash table size: " << ((nb_entree * sizeof(Bitboard) * 4) / (1024 * 1024)) << "m" << endl;
  cout << "Mask: 0x" << hex << mask_cle << dec << endl;
}

void createTranspositionTable(std::string size) {
  int l = size.length();
  char unit = size[l - 1];
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
      return;
  }

  createTranspositionTable(stoi(units) * multiplier);
}

void freeTranspositionTable()
{
  if (white_hash_table)
  {
    free(white_hash_table);
    free(black_hash_table);
    white_hash_table = 0;
    black_hash_table = 0;
  }
}

bool transpositionTableCreated()
{
  return white_hash_table;
}

void displayTranspositionStats() {
  printf("Transposition table stats:\n");
  printf("Hit: %d\n", g_transpositionHit);
  printf("Overwrite: %d\n", g_transpositionOverwrite);
  printf("Write: %d\n", g_transpositionWrite);
  printf("Refutation: %d\n", g_transpositionRefutation);
}

void clearStats() {
  g_transpositionHit = 0;
  g_transpositionOverwrite = 0;
  g_transpositionWrite = 0;
  g_transpositionRefutation = 0;
}

uint32 lookup(TChessBoard *cb, int ply, int depth, int wtm, int *alpha, int *beta, int *danger)
{
  hash_entry* pTable;
  short valeur;

  // Blanc ou noir?
  pTable = (wtm) ? white_hash_table : black_hash_table;

  // Retrouver la position dans la table.
  hash_entry* pPosition = &pTable[mask_cle & (int)cb->CleHachage];

  // Est-ce la bonne position?
  if (pPosition->key ^ cb->CleHachage)
  {
    return 0;
  }

  if (GetDepth((pPosition->data)) < depth)
  {
    return 0;
  }
  
  int type = (int)GetType((pPosition->data));

  // Effacer le bit de l'age.
  ClearAge(pPosition->data);

  valeur = (short)GetValeur(pPosition->data);

  *danger = (int)GetDanger(pPosition->data);

  struct DeuxMot
  {
    int mot1;
    int mot2;
  };
  union Donnee
  {
    TMove move;
    DeuxMot deuxmot;
  };
  Donnee coup;
  coup.deuxmot.mot1 = (int)GetCoup(pPosition->data);
  cb->HashMove[ply] = coup.move;
  g_transpositionHit++;
  switch (type)
  {
  case SCORE_EXACTE:
    if (abs(valeur) >= MATE - 100)
    {
      if (valeur > 0)
        valeur -= (ply - 1);
      else
        valeur += (ply - 1);
    }
    *alpha = valeur;
    return SCORE_EXACTE;
  case BORNE_SUPERIEUR:
    if (valeur <= *alpha)
    {
      *alpha = valeur;
      return BORNE_SUPERIEUR;
    }
    return EVITER_NULL;
    break;
  case BORNE_INFERIEUR:
    if (valeur >= *beta)
    {
      *beta = valeur;
      return BORNE_INFERIEUR;
    }
    return EVITER_NULL;
    break;
  }
  return EVITER_NULL;
}

// On utilise cette fonction lorsque un coup refute le coup de la couche
// precedente.
uint32 storeRefutation(TChessBoard *cb, uint32 ply, uint32 depth,
                       uint32 wtm, short valeur, uint32 alpha, uint32 beta,
                       uint32 danger)
{
  hash_entry* pTable = (wtm) ? white_hash_table : black_hash_table;

  // Retrouver la position dans la table.
  hash_entry* pPosition = &pTable[mask_cle & (int)cb->CleHachage];

  if (GetCoup((pPosition->data)))
  {
    g_transpositionOverwrite++;
  } else {
    g_transpositionWrite++;
  }

  // Mettre les informations dans la table.
  pPosition->key = cb->CleHachage;
  StoreValeur(pPosition->data, valeur);
  int type = BORNE_INFERIEUR;
  StoreType(pPosition->data, type);
  StoreDanger(pPosition->data, danger);
  StoreCoup(pPosition->data, 0);
  StoreDepth(pPosition->data, depth);

  return true;
}

// StoreBest est appelle quand tout les noeud d'un coup a ete explore
// et qu'il est temps de renvoyer la valeur du meilleur coup au noeud parent.
uint32 storeBest(TChessBoard *cb, uint32 ply, uint32 depth, uint32 wtm, uint32 alpha, uint32 initial_alpha, uint32 danger)
{
  hash_entry* pTable;

  // Blanc ou noir?
  pTable = (wtm) ? white_hash_table : black_hash_table;

  // Retrouver la position dans la table.
  hash_entry* pPosition = &pTable[mask_cle & (int)cb->CleHachage];

  if (GetCoup(pPosition->data))
  {
    g_transpositionOverwrite++;
  } else {
    g_transpositionWrite++;
  }

  // Mettre les informations dans la table.
  pPosition->key = cb->CleHachage;
  struct DeuxMot
  {
    int mot1;
    int mot2;
  };
  union Donnee
  {
    TMove move;
    DeuxMot deuxmot;
  };
  Donnee coup;
  coup.move = pv[ply][ply];
  StoreCoup(pPosition->data, coup.deuxmot.mot1);
  StoreDepth(pPosition->data, depth);
  int type;
  if (alpha > initial_alpha)
  {
    type = SCORE_EXACTE;
    StoreValeur(pPosition->data, alpha);
  }
  else
  {
    type = BORNE_SUPERIEUR;
    StoreValeur(pPosition->data, alpha);
  }
  StoreType(pPosition->data, type);

  return true;
}

#endif
