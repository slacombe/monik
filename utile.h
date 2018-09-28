//---------------------------------------------------------------------------
#ifndef UtileH
#define UtileH

#include <math.h>
#include <signal.h>

#include "board.h"
#include "chess.h"

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
int DernierBit( Bitboard bitboard );

// Retourne le bitboard d'attaque de la position demande.
Bitboard AttaqueRangee( int i_iPosition );
Bitboard AttaqueColonne( int i_iPosition );
Bitboard AttaqueTour(int i_iPosition);
Bitboard AttaqueDame(int i_iPosition);
Bitboard AttaqueFou(int i_iPosition);

unsigned int Random32();
Bitboard Random64();

// Verifier si le joueur est en echec.
int Check(int wtm);

void Swap(TMove& m1, TMove& m2);
int Echange(int source, int dest, int wtm);
Bitboard EchangeRayonX(Bitboard attaque, int source, int direction);

int TrouverDernierJournal();

int GagneOpposition( int doit_joue, int roi_blanc, int roi_noir );

// Des macros pour travailler sur les bits.
#define ClearBit( board, position )       (board &= ~mask[position])

#define ClearR90R( board, position )   (board &= ~mask[ Rotate90RMap[ position ] ])

#define ClearR90L( board, position )   (board &= ~mask[ Rotate90LMap[ position ] ])

#define ClearR45L( board, position )   (board &= ~mask[ Rotate45LRealMap[ position ] ])

#define ClearR45R( board, position )   (board &= ~mask[ Rotate45RRealMap[ position ] ])

#define SetBit( board, position )         (board |= mask[position])

#define SetR90R( board, position )     (board |= mask[ Rotate90RMap[ position ] ])

#define SetR90L( board, position )     (board |= mask[ Rotate90LMap[ position ] ])

#define SetR45L( board, position )     (board |= mask[ Rotate45LRealMap[ position ] ])

#define SetR45R( board, position )     (board |= mask[ Rotate45RRealMap[ position ] ])

#define MovePiece( board, move )       ClearBit( board, move.From ); SetBit( board, move.To )

#define MovePieceR90R( board, move )   ClearR90( board, move.From );   SetR90( board, move.To )

#define MovePieceR90L( board, move )   ClearR90L( board, move.From ); SetR90L( board, move.To )

#define MovePieceR45L( board, move )   ClearR45L( board, move.From ); SetR45L( board, move.To )

#define MovePieceR45R( board, move )   ClearR45R( board, move.From ); SetR45R( board, move.To )

#define CalculX( pos )   (pos % 8)
#define CalculY( pos )   (pos / 8)
#define Carre( valeur )  (Valeur*Valeur)
#define DistanceX( v1, v2 )   ( abs( CalculX( v2 ) - CalculX( v1 ) ) )
#define DistanceY( v1, v2 )   ( abs( CalculY( v2 ) - CalculY( v1 ) ) )

#define File( pos )   ((pos)&(0x07))
#define Rank( pos )   ((pos>>3)&(0x07))
#define FileDistance(a,b) abs(((a)&7) - ((b)&7))
#define RankDistance(a,b) abs(((a)>>3) - ((b)>>3))

#define min( x, y )   ((x>y)?y:x)

// En colonne et en rangee.
#define Distance( pos1, pos2 )   ( DistanceX( pos1, pos2 ) + DistanceY( pos1, pos2 ) )

// Macro qui determine si c'est le temp d'appeler quiescence ou
// encore Search.
#define ABSearch(depth, ply, wtm, alpha, beta, do_null ) (depth<=0?Quiescence(ply, wtm, alpha, beta ):Search(depth, ply, wtm, alpha, beta, do_null))

// Macro qui OR les bitboars.
#define Cavalier ((cb.cavalierb | cb.cavaliern))
#define Fou      ((cb.foub | cb.foun))
#define Tour     ((cb.tourb | cb.tourn))
#define Dame     ((cb.dameb | cb.damen))
#define DameFou  ((Dame | Fou))
#define DameTour ((Dame | Tour))

#define And( x, y )  ((x & y))
#define Or( x, y )   ((x | y))
#define Xor( x, y )  ((x ^ y))


#define FILEA		0
#define FILEB		1
#define FILEC		2
#define FILED		3
#define FILEE		4
#define FILEF		5
#define FILEG		6
#define FILEH		7

#define RANK8		0
#define RANK7		1
#define RANK6		2
#define RANK5		3
#define RANK4		4
#define RANK3		5
#define RANK2		6
#define RANK1		7

#define		RoqueBlancDame(x)		(cb.Roque[ply]&ROQUEDAMEBLANC)
#define		RoqueBlancRoi(x)		(cb.Roque[ply]&ROQUEROIBLANC)
#define		RoqueNoirDame(x)		(cb.Roque[ply]&ROQUEDAMENOIR)
#define		RoqueNoirRoi(x)			(cb.Roque[ply]&ROQUEROINOIR)

#define RoqueBlanc(x) And(RoqueBlancDame(x),RoqueBlancRoi(x))
#define	RoqueNoir(x)  And(RoqueNoirDame(x),RoqueNoirRoi(x))

class TSeeker
{
	static void parseseekfile();
public:
	static void start();
	static void stop();
	static void handler_alarm( int signum );
};


//---------------------------------------------------------------------------
#endif
