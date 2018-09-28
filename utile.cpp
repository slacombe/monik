//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>


#include "utile.h"
#include "chess.h"
#include "attaque.h"
#include "make.h"
#include "unmake.h"
#include "journal.h"


// Les bitboards.
// Masque chaque position.
Bitboard mask[BOARDSIZE];

// Table pour pivoter de 90 degree dans le sens des aiguilles.
int Rotate90RMap[] =
  { H8,H7,H6,H5,H4,H3,H2,H1,
    G8,G7,G6,G5,G4,G3,G2,G1,
    F8,F7,F6,F5,F4,F3,F2,F1,
    E8,E7,E6,E5,E4,E3,E2,E1,
    D8,D7,D6,D5,D4,D3,D2,D1,
    C8,C7,C6,C5,C4,C3,C2,C1,
    B8,B7,B6,B5,B4,B3,B2,B1,
    A8,A7,A6,A5,A4,A3,A2,A1 };

// Table pour pivoter de 90 degree dans le sens inverses des aiguilles.
int Rotate90LMap[] =
  { A1,A2,A3,A4,A5,A6,A7,A8,
    B1,B2,B3,B4,B5,B6,B7,B8,
    C1,C2,C3,C4,C5,C6,C7,C8,
    D1,D2,D3,D4,D5,D6,D7,D8,
    E1,E2,E3,E4,E5,E6,E7,E8,
    F1,F2,F3,F4,F5,F6,F7,F8,
    G1,G2,G3,G4,G5,G6,G7,G8,
    H1,H2,H3,H4,H5,H6,H7,H8 };


int Rotate45RMap[64] =
  {                  H8,
                   G8, H7,
                 F8, G7, H6,
               E8, F7, G6, H5,
             D8, E7, F6, G5, H4,
           C8, D7, E6, F5, G4, H3,
         B8, C7, D6, E5, F4, G3, H2,
       A8, B7, C6, D5, E4, F3, G2, H1,
         A7, B6, C5, D4, E3, F2, G1,
           A6, B5, C4, D3, E2, F1,
             A5, B4, C3, D2, E1,
               A4, B3, C2, D1,
                 A3, B2, C1,
                   A2, B1,
                     A1 };

int Rotate45LMap[] =
  {                  A8,
                   A7, B8,
                 A6, B7, C8,
               A5, B6, C7, D8,
             A4, B5, C6, D7, E8,
           A3, B4, C5, D6, E7, F8,
         A2, B3, C4, D5, E6, F7, G8,
       A1, B2, C3, D4, E5, F6, G7, H8,
         B1, C2, D3, E4, F5, G6, H7,
           C1, D2, E3, F4, G5, H6,
             D1, E2, F3, G4, H5,
               E1, F2, G3, H4,
                 F1, G2, H3,
                   G1, H2,
                     H1 };

int Rotate45RRealMap[] =
 {  28,21,15,10, 6, 3, 1, 0,
    36,29,22,16,11, 7, 4, 2,
    43,37,30,23,17,12, 8, 5,
    49,44,38,31,24,18,13, 9,
    54,50,45,39,32,25,19,14,
    58,55,51,46,40,33,26,20,
    61,59,56,52,47,41,34,27,
    63,62,60,57,53,48,42,35 };

/*int Rotate45LRealMap[] =
{    0, 1, 3, 6,10,15,21,28,
     2, 4, 7,11,16,22,29,36,
     5, 8,12,17,23,30,37,43,
     9,13,18,24,31,38,44,49,
    14,19,25,32,39,45,50,54,
    20,26,33,40,46,51,55,58,
    27,34,41,47,52,56,59,61,
    35,42,48,53,57,60,62,63 };*/

int Rotate45LRealMap[] =
{    0, 2, 5, 9,14,20,27,35,
     1, 4, 8,13,19,26,34,42,
     3, 7,12,18,25,33,41,48,
     6,11,17,24,32,40,47,53,
    10,16,23,31,39,46,52,57,
    15,22,30,38,45,51,56,60,
    21,29,37,44,50,55,59,62,
    28,36,43,49,54,58,61,63 };

int DecalageDiagonaleR[] =
  { 28, 21, 15, 10,  6,  3,  1,  0,
    36, 28, 21, 15, 10,  6,  3,  1,
    43, 36, 28, 21, 15, 10,  6,  3,
    49, 43, 36, 28, 21, 15, 10,  6,
    54, 49, 43, 36, 28, 21, 15, 10,
    58, 54, 49, 43, 36, 28, 21, 15,
    61, 58, 54, 49, 43, 36, 28, 21,
    63, 61, 58, 54, 49, 43, 36, 28 };

int DecalageDiagonaleL[] =
  {  0,  1,  3,  6, 10, 15, 21, 28,
     1,  3,  6, 10, 15, 21, 28, 36,
     3,  6, 10, 15, 21, 28, 36, 43,
     6, 10, 15, 21, 28, 36, 43, 49,
    10, 15, 21, 28, 36, 43, 49, 54,
    15, 21, 28, 36, 43, 49, 54, 58,
    21, 28, 36, 43, 49, 54, 58, 61,
    28, 36, 43, 49, 54, 58, 61, 63 };


int LongueurDiagonaleA8_H1[] =
{   8,7,6,5,4,3,2,1,
    7,8,7,6,5,4,3,2,
    6,7,8,7,6,5,4,3,
    5,6,7,8,7,6,5,4,
    4,5,6,7,8,7,6,5,
    3,4,5,6,7,8,7,6,
    2,3,4,5,6,7,8,7,
    1,2,3,4,5,6,7,8 };

int LongueurDiagonaleH8_A1[] =
{   1,2,3,4,5,6,7,8,
    2,3,4,5,6,7,8,7,
    3,4,5,6,7,8,7,6,
    4,5,6,7,8,7,6,5,
    5,6,7,8,7,6,5,4,
    6,7,8,7,6,5,4,3,
    7,8,7,6,5,4,3,2,
    8,7,6,5,4,3,2,1 };

int PositionDansDiagA8_H1[] =
  { 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 2, 2, 2, 2, 2, 2,
    0, 1, 2, 3, 3, 3, 3, 3,
    0, 1, 2, 3, 4, 4, 4, 4,
    0, 1, 2, 3, 4, 5, 5, 5,
    0, 1, 2, 3, 4, 5, 6, 6,
    0, 1, 2, 3, 4, 5, 6, 7 };

int PositionDansDiagH8_A1[] =
  { 0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 6,
    0, 1, 2, 3, 4, 5, 5, 5,
    0, 1, 2, 3, 4, 4, 4, 4,
    0, 1, 2, 3, 3, 3, 3, 3,
    0, 1, 2, 2, 2, 2, 2, 2,
    0, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0 };

int PromoteExtension[] =
{ 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0 };

  

/*

  Code pris dans Crafty.

A 32 bit random number generator. An implementation in C of the algorithm given by
Knuth, the art of computer programming, vol. 2, pp. 26-27. We use e=32, so
we have to evaluate y(n) = y(n - 24) + y(n - 55) mod 2^32, which is implicitly
done by unsigned arithmetic.

*/

unsigned int Random32()
{
  /*
  random numbers from Mathematica 2.0.
  SeedRandom = 1;
  Table[Random[Integer, {0, 2^32 - 1}]
  */
  static unsigned long x[55] = {
    1410651636UL, 3012776752UL, 3497475623UL, 2892145026UL, 1571949714UL,
    3253082284UL, 3489895018UL,  387949491UL, 2597396737UL, 1981903553UL,
    3160251843UL,  129444464UL, 1851443344UL, 4156445905UL,  224604922UL,
    1455067070UL, 3953493484UL, 1460937157UL, 2528362617UL,  317430674UL,
    3229354360UL,  117491133UL,  832845075UL, 1961600170UL, 1321557429UL,
     747750121UL,  545747446UL,  810476036UL,  503334515UL, 4088144633UL,
    2824216555UL, 3738252341UL, 3493754131UL, 3672533954UL,   29494241UL,
    1180928407UL, 4213624418UL,   33062851UL, 3221315737UL, 1145213552UL,
    2957984897UL, 4078668503UL, 2262661702UL,   65478801UL, 2527208841UL,
    1960622036UL,  315685891UL, 1196037864UL,  804614524UL, 1421733266UL,
    2017105031UL, 3882325900UL,  810735053UL,  384606609UL, 2393861397UL };
  static int init = 1;
  static unsigned long y[55];
  static int j, k;
  unsigned long ul;

  if (init)
  {
    int i;

    init = 0;
    for (i = 0; i < 55; i++) y[i] = x[i];
    j = 24 - 1;
    k = 55 - 1;
  }

  ul = (y[k] += y[j]);
  if (--j < 0) j = 55 - 1;
  if (--k < 0) k = 55 - 1;
  return((unsigned int)ul);
}

Bitboard Random64()
{
  Bitboard result;
  unsigned int r1, r2;

  r1=Random32();
  r2=Random32();
  result = r1 | ((Bitboard)r2 << 32);
  return (result);
}

// Retourne la position du dernier bit d'un entier de 64 bits.
int DernierBit( Bitboard bitboard )
{
  // On definit une structure qui nous permet d'aller chercher la bonne
  // partie.
  union BitboardSplit
  {
    Bitboard b;
    unsigned short s[4];
    unsigned int i[2];
  };

  register BitboardSplit split;
  split.b = bitboard;

  // Est-ce qu'il y a un bit dans le 32 bits de poid fort?
  if ( split.i[1] ) {
    // Est-ce qu'il est situe dans le premier 16 bits?
    if ( split.s[2] )
      return dernier_bits[ split.s[2] ]+32;
    else
      return dernier_bits[ split.s[3] ]+48;
  }
  else {
    // Est-ce qu'il est situe dans le premier 16 bits?
    if ( split.s[0] )
      return dernier_bits[ split.s[0] ];
    else
      return dernier_bits[ split.s[1] ]+16;
  }
}

Bitboard AttaqueRangee( int i_iPosition )
{
	int iRangee = i_iPosition/8;
	Bitboard tempR = cb.piece >> iRangee*8;

	return cb.AttRangee[i_iPosition][(unsigned char)tempR];
}

Bitboard AttaqueColonne( int i_iPosition )
{
	int iColonne = i_iPosition&7;
	Bitboard tempC = cb.pieceR90L >> (7-iColonne)*8;

	return cb.AttColonne[i_iPosition][(unsigned char)tempC];
}

// Retourne le bitboard d'attaque de la position demandee.
Bitboard AttaqueTour(int i_iPosition)
{
  // Quelle rangee?
  int iRangee = i_iPosition/8;
  int iColonne = i_iPosition&7;
  Bitboard tempR = cb.piece >> iRangee*8;
  Bitboard tempC = cb.pieceR90L >> (7-iColonne)*8;

  unsigned char cPatternR = (char)tempR;
  unsigned char cPatternC = (char)tempC;

  Bitboard att = cb.AttRangee[i_iPosition][cPatternR];
  att |= cb.AttColonne[i_iPosition][cPatternC];

  return att;
}

// Retourne le bitboard d'attaque de la position demande.
Bitboard AttaqueDame(int i_iPosition)
{
  return AttaqueTour(i_iPosition) |
         AttaqueFou(i_iPosition);
}

// Les Fous.
// Retourne le bitboard d'attaque de la position demande.
Bitboard AttaqueFou(int i_iPosition)
{
  Bitboard tempD =
    cb.pieceR45R >> DecalageDiagonaleR[i_iPosition];

  unsigned char cPatternD = (char)tempD;
  cPatternD &= (char)((1 << LongueurDiagonaleA8_H1[i_iPosition])-1);

  Bitboard att = cb.AttDiagonaleA8_H1[i_iPosition][cPatternD];

  tempD = cb.pieceR45L >> DecalageDiagonaleL[i_iPosition];

  cPatternD = (char)tempD;
  cPatternD &= (char)((1 << LongueurDiagonaleH8_A1[i_iPosition])-1);

  att |= cb.AttDiagonaleH8_A1[i_iPosition][cPatternD];

  return att;
}

// Verifie si le joueur est en echec.
int Check(int wtm)
{
  if ( wtm ) {
    return Attacked(cb.PositionRoiBlanc, !wtm);
  }
  else {
    return Attacked(cb.PositionRoiNoir, !wtm);
  }
}

void Swap( TMove& m1, TMove& m2 )
{
  TMove T = m1;
  m1 = m2;
  m2 = T;
}

// Echange retourne la valeur d'un echange. Le but de cette fonction
// est d'accelerer Quiescence en ne considerant que les echanges qui peuvent
// ameliorer le score.
int Echange(int source, int destination, int wtm)
{
  register Bitboard attaques;
  register bool bCote;
  register int direction, piececapture, capture, signe;

  // Effectuer la premiere capture pour partir l'echange.
  signe=1;
  bCote = !wtm;

  // Prochaine piece a etre capturee.
  piececapture = abs(cb.board[destination]);

  // Valeur de la piece.
  capture = ValeurPiece[piececapture];

  // Les pieces qui peuvent attaquer destination.
  attaques = AttaqueDe(destination, wtm) |
             AttaqueDe(destination, !wtm);

  // La direction de la capture.
  direction = cb.directions[source][destination];

  ClearBit( attaques, source );

  // Regarder derriere la piece qui capture pour voir si une
  // autre piece vient de se decouvrir et peut attaquer destination.
  if ( direction )
    attaques = EchangeRayonX(attaques, source, direction);

  while( attaques ) {
    signe = -signe;
    piececapture = abs(cb.board[source]);

    if ( bCote ) {
      if ( attaques & cb.pionb )
        source = DernierBit( attaques & cb.pionb );
      else if ( attaques & cb.cavalierb )
        source = DernierBit( attaques & cb.cavalierb );
      else if ( attaques & cb.foub )
        source = DernierBit( attaques & cb.foub );
      else if ( attaques & cb.tourb )
        source = DernierBit( attaques & cb.tourb );
      else if ( attaques & cb.dameb )
        source = DernierBit( attaques & cb.dameb );
      else if ( attaques & cb.roib )
        source = DernierBit( attaques & cb.roib );
      else break;
    }
    else {
      if ( attaques & cb.pionn )
        source = DernierBit( attaques & cb.pionn );
      else if ( attaques & cb.cavaliern )
        source = DernierBit( attaques & cb.cavaliern );
      else if ( attaques & cb.foun )
        source = DernierBit( attaques & cb.foun );
      else if ( attaques & cb.tourn )
        source = DernierBit( attaques & cb.tourn );
      else if ( attaques & cb.damen )
        source = DernierBit( attaques & cb.damen );
      else if ( attaques & cb.roin )
        source = DernierBit( attaques & cb.roin );
      else break;
    }

    bCote = !bCote;
    capture += ValeurPiece[piececapture]*signe;

    ClearBit( attaques, source );
    direction = cb.directions[source][destination];
    if ( direction )
      attaques = EchangeRayonX(attaques, source, direction );

  } // while

  // Retourne la valeur de l'echange.
  return capture;
}

// Fonction qui regarde si il y a une piece en arriere et si il y en a
// une elle regarde si elle peut capturer la source. Si elle le peut,
// l'ajouter a la liste des echanges.
Bitboard EchangeRayonX(Bitboard attaque, int source, int direction)
{
  switch( direction ) {
    case -1:
      return Or( attaque,
                 And( And( AttaqueTour(source), DameTour ),
                      cb.plus1dir[source]));
    case -7:
      return Or( attaque,
                 And( And( AttaqueFou(source), DameFou ),
                      cb.plus7dir[source]));
    case -8:
      return Or( attaque,
                 And( And( AttaqueTour(source), DameTour ),
                      cb.plus8dir[source]));
    case -9:
      return Or( attaque,
                 And( And( AttaqueFou(source ), DameFou),
                      cb.plus9dir[source]));
    case 1:
      return Or( attaque,
                 And( And( AttaqueTour(source), DameTour ),
                      cb.minus1dir[source]));
    case 7:
      return Or( attaque,
                 And( And( AttaqueFou(source), DameFou ),
                      cb.minus7dir[source]));
    case 8:
      return Or( attaque,
                 And( And( AttaqueTour(source), DameTour ),
                      cb.minus8dir[source]));
    case 9:
      return Or( attaque,
                 And( And( AttaqueFou(source), DameFou ),
                      cb.minus9dir[source]));
  }
  return attaque;
}

int GagneOpposition(int doit_joue, int roi_blanc, int roi_noir)
{
	register int file_distance, rank_distance;
	file_distance = FileDistance( roi_blanc, roi_noir );
	rank_distance = RankDistance( roi_blanc, roi_noir );
	if ( rank_distance < 2 ) return 1;
	if ( doit_joue ) {
		if ( rank_distance&1 ) {
			rank_distance--;
			if( file_distance&1) file_distance--;
		}
		else if ( file_distance&1 ) {
			file_distance--;
			if (rank_distance&1) rank_distance--;
		}
	}
	if ( !(file_distance&1) && !(rank_distance&1)) return 1;
	return 0;
}

void TSeeker::parseseekfile()
{
	/*
	FILE* fp = fopen( "seek.cmd", "rb" );
	if ( !fp ) {
		return;
	}

	// If command too long do not send it.
	char seekcmd[80];
	while( !feof( fp ) ) {
		fgets( seekcmd, sizeof( seekcmd )-1, fp );
		seekcmd[sizeof(seekcmd)-1] = 0;
		printf( "tellics %s\n", seekcmd );
	}
	fclose( fp );
	*/
}

void TSeeker::start()
{
	/*
	signal( SIGALRM, handler_alarm );
	alarm( 5*60 );
	parseseekfile();
	journal.Log( "Seeker started" );
	*/
}

void TSeeker::stop()
{
	/*
	signal( SIGALRM, SIG_IGN );
	alarm( 0 );
	journal.Log( "Seeker stopped" );
	*/
}

void TSeeker::handler_alarm( int signum )
{
	/*
	parseseekfile();
	alarm( 5*60 );
	*/
}
//---------------------------------------------------------------------------

