//---------------------------------------------------------------------------
#ifndef TranspositionH
#define TranspositionH

#include "stdafx.h"

#include "chess.h"

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


#define SetAge( x ) \
  (x |= maskAge)
#define ClearAge( x ) \
  (x &= (~maskAge))
#define GetAge( x ) \
  (x & maskAge)
#define StoreType( x, t ) \
  (x |= ((Bitboard)(t&3) << 61))
#define GetType( x ) \
  (((Bitboard)x >> 61) & 0x03)
#define StoreDanger( x, d ) \
  (x |= ((Bitboard) (d&1) << 60))
#define GetDanger( x ) \
  ((x & maskDanger) >> 60)
#define StoreValeur( x, v ) \
  (x |= ((Bitboard) ((short)(v)&0xFFFF) << 44))
#define GetValeur( x ) \
  (((short)(x >> 44) & 0xFFFF))
#define StoreCoup( x, c ) \
  (x |= ((Bitboard) ((c)&0xFFFFFFFF) << 12))
#define GetCoup( x ) \
  ((x & maskCoup) >> 12)
#define StoreDepth( x, d ) \
  (x |= ((Bitboard) (d&0xFFF)))
#define GetDepth( x ) \
  (x & 0xFFF)

class TTableTrans {
  Bitboard* m_pTableBlanc;
  Bitboard* m_pTableNoir;

  int m_iNbEntree;
  int m_iMaskCle;
public:
  // Constructeur par defaut.
  TTableTrans();

  // Constructeur avec passage de la taille en byte disponible.
  TTableTrans( int i_iTaille );

  // Destructeur.
  ~TTableTrans();

  // Initialise les deux tables.
  // Mettre le flag a 1.
  void Initialise();

  // Reinitialise la table de transposition.
  // Mettre l'age a 1.
  void Reinitialise();

  // Chercher une position dans la table.
  int Lookup( TChessBoard& cb, int ply, int depth,
              int wtm, int& alpha, int& beta, int& danger );

  int StoreRefutation( TChessBoard& cb, int ply, int depth, int wtm,
                       short valeur, int alpha, int beta, int danger );

  int StoreBest( TChessBoard& cb, int ply, int depth, int wtm,
                 int alpha, int initial_alpha, int danger );
};

extern TTableTrans *TableTrans;

#endif

//---------------------------------------------------------------------------
#endif
