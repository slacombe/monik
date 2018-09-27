#ifdef TRANSPOSITION

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "transposition.h"

TTableTrans *TableTrans;

int g_iTranspositionHit;
int g_iTranspositionCollision;
int g_iRefutation;

// Constructeur avec passage de la taille en megabyte disponible.
TTableTrans::TTableTrans( int i_iTaille )
{
  // La taille en octet.
  int iTaille = i_iTaille*1024*1024;


  // Nombre d'entree maximum qu'on peut entrer dans cette espace.
  int iEntreeMax = iTaille / (sizeof( Bitboard )*2);

  // Reconvertir en mega-octets.
  int iNbEntreeBase2 = 1;
  while( iEntreeMax ) {
    iEntreeMax >>= 1;
    iNbEntreeBase2 <<= 1;
  }
  // Une de trop et diviser en deux, une moitie pour chaque table.
  iNbEntreeBase2 >>= 2;

  // Alouer la memoire necessaire.
  m_pTableBlanc = (Bitboard*)malloc( iNbEntreeBase2*sizeof(Bitboard)*2 );
  m_pTableNoir = (Bitboard*)malloc( iNbEntreeBase2*sizeof(Bitboard)*2 );

  m_iNbEntree = iNbEntreeBase2;
  m_iMaskCle = m_iNbEntree - 1;

  printf( "Tranposition: %d positions\n", m_iNbEntree*2 );
  printf( "Hash table size: %dk\n", (m_iNbEntree*sizeof(Bitboard)*4)/1024);
  printf( "Mask: 0x%x\n", m_iMaskCle );
}

TTableTrans::~TTableTrans()
{
  free( m_pTableBlanc );
  free( m_pTableNoir );
}

void TTableTrans::Initialise()
{
  memset( m_pTableBlanc, 0, m_iNbEntree*2*sizeof(Bitboard) );
  memset( m_pTableNoir, 0, m_iNbEntree*2*sizeof(Bitboard) );
}

void TTableTrans::Reinitialise()
{
  register Bitboard* entree;
  register int i;
  entree = m_pTableBlanc;
  for( i=0; i<m_iNbEntree; i++ ) {
    SetAge( (*entree) );
    entree += 2;
  }
  entree = m_pTableNoir;
  for( i=0; i<m_iNbEntree; i++ ) {
    SetAge( (*entree) );
    entree += 2;
  }
}

int TTableTrans::Lookup( TChessBoard& cb, int ply, int depth,
                         int wtm, int& alpha, int& beta, int& danger )
{
  Bitboard* pTable;
  short valeur;

  // Blanc ou noir?
  pTable = (wtm)?m_pTableBlanc:m_pTableNoir;

  // Retrouver la position dans la table.
  int  iPosition = (m_iMaskCle & (int)cb.CleHachage);
  pTable += iPosition*2;

  // Est-ce la bonne position?
  if ( (*(pTable+1) ^ cb.CleHachage) ) {
    return 0;
  }

  int iType;
  int DepthValide = (int)GetDepth( (*pTable) );
  if ( DepthValide < depth ) {
	return 0;
  }
  else
    iType = (int)GetType( (*pTable) );

  // Effacer le bit de l'age.
  ClearAge( (*pTable) );

  valeur = (short)GetValeur( (*pTable) );

  danger = (int)GetDanger( (*pTable) );

  struct DeuxMot {
    int mot1;
    int mot2;
  };
  union Donnee {
    TMove move;
    DeuxMot deuxmot;
  };
  Donnee coup;
  coup.deuxmot.mot1 = (int)GetCoup( (*pTable) );
  cb.HashMove[ply] = coup.move;
  g_iTranspositionHit++;
  switch( iType ) {
    case SCORE_EXACTE:
      if ( abs( valeur ) >= MATE-100 ) {
        if ( valeur > 0 ) valeur -= (ply-1);
        else valeur += (ply-1);
      }
      alpha = valeur;
      return SCORE_EXACTE;
    case BORNE_SUPERIEUR:
      if ( valeur <= alpha ) {
        alpha = valeur;
        return BORNE_SUPERIEUR;
      }
	  return EVITER_NULL;
      break;
    case BORNE_INFERIEUR:
      if ( valeur >= beta ) {
        beta = valeur;
        return BORNE_INFERIEUR;
      }
	  return EVITER_NULL;
      break;
  }
  return EVITER_NULL;
}

// On utilise cette fonction lorsque un coup refute le coup de la couche
// precedente.
int TTableTrans::StoreRefutation( TChessBoard& cb, int ply, int depth,
                                  int wtm, short valeur, int alpha, int beta,
                                  int danger )
{
  Bitboard* pTable;

  // Blanc ou noir?
  pTable = (wtm)?m_pTableBlanc:m_pTableNoir;

  // Retrouver la position dans la table.
  int  iPosition = (m_iMaskCle & (int)cb.CleHachage);
  pTable += iPosition*2;


  // Verifier si la position est meilleur que celle qu'on a dans la table.
  // Si l'age est setter alors toujours remplacer.
  if ( !GetAge( (*pTable) ) && GetDepth( (*pTable) ) > depth  ) {
	g_iTranspositionCollision++;
    return false;
  }

  // Mettre les informations dans la table.
  *pTable = 0;
  *(pTable+1) = cb.CleHachage;
  StoreValeur( (*pTable), valeur );
  int iType = BORNE_INFERIEUR;
  StoreType( (*pTable), iType );
  StoreDanger( (*pTable), danger );
  StoreCoup( (*pTable), 0 );
  StoreDepth( (*pTable), depth );

  return true;
}

// StoreBest est appelle quand tout les noeud d'un coup a ete explore
// et qu'il est temps de renvoyer la valeur du meilleur coup au noeud parent.
int TTableTrans::StoreBest( TChessBoard& cb, int ply, int depth,
                            int wtm, int alpha, int initial_alpha, int danger )
{
  Bitboard* pTable;

  // Blanc ou noir?
  pTable = (wtm)?m_pTableBlanc:m_pTableNoir;

  // Retrouver la position dans la table.
  int  iPosition = (m_iMaskCle & (int)cb.CleHachage);
  pTable += iPosition*2;


  // Verifier si la position est meilleur que celle qu'on a dans la table.
  // Si l'age est setter alors toujours remplacer.
  if ( !GetAge( (*pTable) ) && GetDepth( (*pTable) ) >= depth  )
    return false;

  // Mettre les informations dans la table.
  *pTable = 0;
  *(pTable+1) = cb.CleHachage;
  struct DeuxMot {
    int mot1;
    int mot2;
  };
  union Donnee {
    TMove move;
    DeuxMot deuxmot;
  };
  Donnee coup;
  coup.move = pv[ply][ply];
  StoreCoup( (*pTable), coup.deuxmot.mot1 );
  StoreDepth( (*pTable), depth );
  int iType;
  if ( alpha > initial_alpha ) {
    iType = SCORE_EXACTE;
    StoreValeur( (*pTable), alpha );
  }
  else {
    iType = BORNE_SUPERIEUR;
    StoreValeur( (*pTable), alpha );
  }
  StoreType( (*pTable), iType );

  return true;
}
#endif
