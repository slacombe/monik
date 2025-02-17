#ifdef TRANSPOSITION

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "transposition.h"

using namespace std;

int g_iTranspositionHit;
int g_iTranspositionCollision;
int g_iRefutation;

uint32 nbEntree;
uint32 maskCle;

Bitboard *whiteTranspositionTable = 0;
Bitboard *blackTranspositionTable = 0;

void createTranspositionTable(uint32 sizeInMeg)
{
  int sizeInBytes = sizeInMeg * 1024 * 1024;

  int nbOfPositionsInTable = sizeInBytes / (sizeof(Bitboard) * 2);

  // Reconvertir en mega-octets.
  int iNbEntreeBase2 = 1;
  while (nbOfPositionsInTable)
  {
    nbOfPositionsInTable >>= 1;
    iNbEntreeBase2 <<= 1;
  }
  // Une de trop et diviser en deux, une moitie pour chaque table.
  iNbEntreeBase2 >>= 2;

  // Alouer la memoire necessaire.
  whiteTranspositionTable = (Bitboard *)malloc(iNbEntreeBase2 * sizeof(Bitboard) * 2);
  blackTranspositionTable = (Bitboard *)malloc(iNbEntreeBase2 * sizeof(Bitboard) * 2);

  nbEntree = iNbEntreeBase2;
  maskCle = nbEntree - 1;

  printf("Tranposition: %lu positions\n",  nbEntree * 2);
  printf("Hash table size: %lum\n", ((nbEntree * sizeof(Bitboard) * 4) / (1024 * 1024)));
  printf("Mask: 0x%lx\n", maskCle);
}

void freeTranspositionTable()
{
  if (whiteTranspositionTable)
  {
    free(whiteTranspositionTable);
    free(blackTranspositionTable);
    whiteTranspositionTable = 0;
    blackTranspositionTable = 0;
  }
}

bool transpositionTableCreated()
{
  return whiteTranspositionTable;
}

void initializeTranspositionTable()
{
  memset(whiteTranspositionTable, 0, nbEntree * 2 * sizeof(Bitboard));
  memset(blackTranspositionTable, 0, nbEntree * 2 * sizeof(Bitboard));
}

void reinitialize()
{
  Bitboard *entree;

  entree = whiteTranspositionTable;
  for (uint32 i = 0; i < nbEntree; i++)
  {
    SetAge((*entree));
    entree += 2;
  }
  entree = blackTranspositionTable;
  for (uint32 i = 0; i < nbEntree; i++)
  {
    SetAge((*entree));
    entree += 2;
  }
}

uint32 lookup(TChessBoard *cb, int ply, int depth, int wtm, int *alpha, int *beta, int *danger)
{
  Bitboard *pTable;
  short valeur;

  // Blanc ou noir?
  pTable = (wtm) ? whiteTranspositionTable : blackTranspositionTable;

  // Retrouver la position dans la table.
  int iPosition = (maskCle & (int)cb->CleHachage);
  pTable += iPosition * 2;

  // Est-ce la bonne position?
  if ((*(pTable + 1) ^ cb->CleHachage))
  {
    return 0;
  }

  int iType;
  int DepthValide = (int)GetDepth((*pTable));
  if (DepthValide < depth)
  {
    return 0;
  }
  else
    iType = (int)GetType((*pTable));

  // Effacer le bit de l'age.
  ClearAge((*pTable));

  valeur = (short)GetValeur((*pTable));

  *danger = (int)GetDanger((*pTable));

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
  coup.deuxmot.mot1 = (int)GetCoup((*pTable));
  cb->HashMove[ply] = coup.move;
  g_iTranspositionHit++;
  switch (iType)
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
  Bitboard *pTable = (wtm) ? whiteTranspositionTable : blackTranspositionTable;

  // Retrouver la position dans la table.
  int iPosition = (maskCle & (int)cb->CleHachage);
  Bitboard* pPosition = pTable + iPosition * 2;

  // Verifier si la position est meilleur que celle qu'on a dans la table.
  // Si l'age est setter alors toujours remplacer.
  if (!GetAge((*pTable)) && GetDepth((*pPosition)) > depth)
  {
    g_iTranspositionCollision++;
    return false;
  }

  // Mettre les informations dans la table.
  *pPosition = 0;
  *(pPosition + 1) = cb->CleHachage;
  StoreValeur((*pPosition), valeur);
  int iType = BORNE_INFERIEUR;
  StoreType((*pPosition), iType);
  StoreDanger((*pPosition), danger);
  StoreCoup((*pPosition), 0);
  StoreDepth((*pPosition), depth);

  return true;
}

// StoreBest est appelle quand tout les noeud d'un coup a ete explore
// et qu'il est temps de renvoyer la valeur du meilleur coup au noeud parent.
uint32 storeBest(TChessBoard *cb, uint32 ply, uint32 depth, uint32 wtm, uint32 alpha, uint32 initial_alpha, uint32 danger)
{
  Bitboard *pTable;

  // Blanc ou noir?
  pTable = (wtm) ? whiteTranspositionTable : blackTranspositionTable;

  // Retrouver la position dans la table.
  int iPosition = (maskCle & (int)cb->CleHachage);
  pTable += iPosition * 2;

  // Verifier si la position est meilleur que celle qu'on a dans la table.
  // Si l'age est setter alors toujours remplacer.
  if (!GetAge((*pTable)) && GetDepth((*pTable)) >= depth)
    return false;

  // Mettre les informations dans la table.
  *pTable = 0;
  *(pTable + 1) = cb->CleHachage;
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
  StoreCoup((*pTable), coup.deuxmot.mot1);
  StoreDepth((*pTable), depth);
  int iType;
  if (alpha > initial_alpha)
  {
    iType = SCORE_EXACTE;
    StoreValeur((*pTable), alpha);
  }
  else
  {
    iType = BORNE_SUPERIEUR;
    StoreValeur((*pTable), alpha);
  }
  StoreType((*pTable), iType);

  return true;
}
#endif
