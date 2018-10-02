#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chess.h"
#include "eval.h"
#include "utile.h"
#include "attaque.h"
#include "journal.h"

SCORE PionRoiBlanc( int col );
SCORE PionRoiNoir( int col );

extern int root_wtm, iNodes, nbevals;

const SCORE bishop_pair_bonus[9] = { 20, 20, 20, 20, 20, 18, 12, 8 };


const SCORE PionCentre[] =
{
	  0,   0,   0,   0,   0,   0,   0,   0,
	  5,  10,  15,  20,  20,  15,  10,   5,
	  4,   8,  12,  16,  16,  12,   8,   4,
	  3,   6,   9,  10,  10,   9,   6,   3,
	  2,   4,   6,   8,   8,   6,   4,   2,
	  1,   2,   3, -20, -20,   3,   2,   1,
	  0,   0,   0, -40, -40,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0
};

const SCORE PionRoqueRoi[] =
{
	  0,   0,   0,   0,   0,   0,   0,   0,
	  5,  10,  15,  20,  20,  15,  10,   5,
	  4,   8,  12,  16,  16,  12,   8,   4,
	  3,   6,   9,  10,  10,   9,   6,   3,
	  2,   4,   6,   8,   8,   6,   4,   2,
	 -2,  -3,  -1, -20, -20,   3,   2,   1,
	 -5,  -5,  -5, -40, -40,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0
};

const SCORE PionRoqueDame[] =
{
	  0,   0,   0,   0,   0,   0,   0,   0,
	  5,  10,  15,  20,  20,  15,  10,   5,
	  4,   8,  12,  16,  16,  12,   8,   4,
	  3,   6,   9,  10,  10,   9,   6,   3,
	  2,   4,   6,   8,   8,   6,   8,   6,
	  1,   2,   3, -20, -20,  -1,  -3,  -2,
	  0,   0,   0, -40, -40,  -5,  -5,  -5,
	  0,   0,   0,   0,   0,   0,   0,   0
};

const SCORE PositionCavalier[] =
{
	-10, -10, -10, -10, -10, -10, -10, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10, -30, -10, -10, -10, -10, -30, -10
};

const SCORE PositionFou [] =
{
	-10, -10, -10, -10, -10, -10, -10, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10, -10, -20, -10, -10, -20, -10, -10
};

// Ouverture et milieu de partie.
const SCORE PosRoi[] =
{
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-20, -20, -20, -20, -20, -20, -20, -20,
	  0,  30,  20, -20,   0, -20,  30,  30
};

const SCORE PosRoiFin[] = {
	  0,  10,  20,  30,  30,  20,  10,   0,
	 10,  20,  30,  40,  40,  30,  20,  10,
	 20,  30,  40,  50,  50,  40,  30,  20,
	 30,  40,  50,  60,  60,  50,  40,  30,
	 30,  40,  50,  60,  60,  50,  40,  30,
	 20,  30,  40,  50,  50,  40,  30,  20,
	 10,  20,  30,  40,  40,  30,  20,  10,
	  0,  10,  20,  30,  30,  20,  10,   0
};

const int flip[] = {
	 56,  57,  58,  59,  60,  61,  62,  63,
	 48,  49,  50,  51,  52,  53,  54,  55,
	 40,  41,  42,  43,  44,  45,  46,  47,
	 32,  33,  34,  35,  36,  37,  38,  39,
	 24,  25,  26,  27,  28,  29,  30,  31,
	 16,  17,  18,  19,  20,  21,  22,  23,
	  8,   9,  10,  11,  12,  13,  14,  15,
	  0,   1,   2,   3,   4,   5,   6,   7
};

#define BLANC    0
#define NOIR     1
int RangeePion[2][10];

// Fonction d'evaluation. Si les blancs sont en avantage le score sera positif,
// sinon le score sera negatif.
SCORE Eval(int ply, int wtm, SCORE alpha, SCORE beta)
{
  register int i, pos;
  register SCORE Score = EvaluateMaterial(wtm);

  nbevals++;
  iNodes++;

	for( i=0; i<10; i++ ) {
		RangeePion[BLANC][i] = 0;
		RangeePion[NOIR][i] = 7;
	}

	//
	// Pour commencer, calculer la rangee de chaque pions.
	//
	Bitboard pions = cb.pionb | cb.pionn;
	while( pions ) {
		pos = DernierBit(pions);

		int col = File(pos) + 1;

		if ( pions & cb.pionb & mask[pos] ) {
			if ( RangeePion[BLANC][col] < Rank(pos) )
				RangeePion[BLANC][col] = Rank(pos);
		}
		else {
			if ( RangeePion[NOIR][col] > Rank(pos) )
				RangeePion[NOIR][col] = Rank(pos);
		}

		ClearBit( pions, pos );
	}

	// Le roi blanc.
 	if ( cb.ScoreMaterielNoir - (cb.TotalPionNoir * VALPION) <= 1400 ) {
		Score += PosRoiFin[cb.PositionRoiBlanc];
	}
	else {
		Score += ProtectionRoiBlanc(ply);
	}

	// Le roi noir.
	if ( cb.ScoreMaterielBlanc - (cb.TotalPionBlanc * VALPION) <= 1400 ) {
		Score -= PosRoiFin[flip[cb.PositionRoiNoir]];
	}
	else {
		Score -= ProtectionRoiNoir(ply);
	}

	// 
	// Les pions blancs.
	//
	if ( cb.TotalPionBlanc == 8 )
		Score -= ALL_PAWNS_PENALTY;
	pions = cb.pionb;
	while( pions ) {
		pos = DernierBit(pions);

		if (File(cb.PositionRoiBlanc) < 3)
			Score += PionRoqueDame[pos];
		else if (File(cb.PositionRoiBlanc) > 4)
			Score += PionRoqueRoi[pos];
		else
			Score += PionCentre[pos];

		int col = File(pos) + 1;

		// S'il y a un pion deriere nous, c'est un double.
		if ( RangeePion[BLANC][col] > Rank(pos) )
			Score -= DOUBLED_PAWN_PENALTY;

		// S'il n'y a pas de pion a gauche ou a droite alors
		// c'est un pion isole.
		if ((RangeePion[BLANC][col-1] == 0 &&
			 RangeePion[BLANC][col+1] == 0))
			Score -= ISOLATED_PAWN_PENALTY;

		// Si ce n'est pas un pion isole, peut-etre que
		// c'est un pion ariere.
		else if ( (RangeePion[BLANC][col-1] < Rank(pos)) &&
				  (RangeePion[BLANC][col+1] < Rank(pos)))
			Score -= BACKWARDS_PAWN_PENALTY;

		// Ajouter un bonus si c'est un pion passe.
		if (( RangeePion[NOIR][col-1] >= Rank(pos)) &&
			( RangeePion[NOIR][col] >= Rank(pos)) &&
			  RangeePion[NOIR][col+1] >= Rank(pos))
			Score += (7 - Rank(pos)) * PASSED_PAWN_BONUS;

		ClearBit( pions, pos );
	}

	// 
	// Les pions noirs.
	//
	if ( cb.TotalPionNoir == 8 )
		Score += ALL_PAWNS_PENALTY;
	pions = cb.pionn;
	while( pions ) {
		pos = DernierBit(pions);

		if (File(cb.PositionRoiNoir) < 3)
			Score -= PionRoqueDame[flip[pos]];
		else if (File(cb.PositionRoiNoir) > 4)
			Score -= PionRoqueRoi[flip[pos]];
		else
			Score -= PionCentre[flip[pos]];

		int col = File(pos) + 1;

		// S'il y a un pion deriere nous, c'est un double.
		if ( RangeePion[NOIR][col] < Rank(pos) )
			Score += DOUBLED_PAWN_PENALTY;

		// S'il n'y a pas de pion a gauche ou a droite alors
		// c'est un pion isole.
		if ((RangeePion[NOIR][col-1] == 7 &&
			 RangeePion[NOIR][col+1] == 7))
			Score += ISOLATED_PAWN_PENALTY;

		// Si ce n'est pas un pion isole, peut-etre que
		// c'est un pion arriere.
		else if ( (RangeePion[NOIR][col-1] > Rank(pos)) &&
				  (RangeePion[NOIR][col+1] > Rank(pos)))
			Score += BACKWARDS_PAWN_PENALTY;

		// Ajouter un bonus si c'est un pion passe.
		if (( RangeePion[BLANC][col-1] <= Rank(pos)) &&
			( RangeePion[BLANC][col] <= Rank(pos)) &&
			  RangeePion[BLANC][col+1] <= Rank(pos))
			Score -= Rank(pos) * PASSED_PAWN_BONUS;

		ClearBit( pions, pos );
	}

	//
	// Calculer la position des fous.
	//
	// Un bonus pour la pair de fou.
	if ( cb.TotalFouBlanc == 2 && cb.TotalFouNoir < 2 ) {
		Score += bishop_pair_bonus[cb.TotalPionBlanc];
	}
	if ( cb.TotalFouNoir == 2 && cb.TotalFouBlanc < 2 ) {
		Score -= bishop_pair_bonus[cb.TotalPionNoir];
	}

	int to;
	Bitboard temp;
	Bitboard fous = cb.foub;
	while( fous ) {
		pos = DernierBit(fous);

		Score += PositionFou[pos];

		// 1 points pour chaque case attaque.
		temp = (AttaqueFou(pos)&(~cb.pieceb));
		while( temp ) {
			to = DernierBit( temp );
			ClearBit( temp, to );
			Score += 1;
		}

		// Penaliser chaque pion sur sa couleur.
		if ( cb.TotalFouBlanc  == 1 ) {
			Bitboard pions;
			if ( mask[pos] & cb.CaseBlanche ) {
				pions = cb.pionb & cb.CaseBlanche;
			}
			else {
				pions = cb.pionb & cb.CaseNoire;
			}
			while( pions ) {
				to = DernierBit( pions );
				Score += 1;
				ClearBit( pions, to );
			}
		}
		ClearBit( fous, pos );
	}
	fous = cb.foun;
	while( fous ) {
		pos = DernierBit(fous);

		Score -= PositionFou[flip[pos]];
		// 1 points pour chaque case attaque.
		temp = (AttaqueFou(pos)&(~cb.piecen));
		while( temp ) {
			to = DernierBit( temp );
			ClearBit( temp, to );
			Score -= 1;
		}

		// Penaliser chaque pion sur sa couleur.
		if ( cb.TotalFouNoir == 1 ) {
			Bitboard pions;
			if ( mask[pos] & cb.CaseBlanche ) {
				pions = cb.pionn & cb.CaseBlanche;
			}
			else {
				pions = cb.pionn & cb.CaseNoire;
			}
			while( pions ) {
				to = DernierBit( pions );
				Score -= 1;
				ClearBit( pions, to );
			}
		}
		ClearBit( fous, pos );
	}

	//
	// Calculer la position des cavaliers.
	//
	Bitboard cavaliers = cb.cavalierb;
	while( cavaliers ) {
		pos = DernierBit( cavaliers );

		Score += PositionCavalier[pos];

		// Bonus for an outpost.
		int col = File(pos)+1;
		if ( (RangeePion[NOIR][col-1] >= Rank(pos)) &&
			 (RangeePion[NOIR][col+1] >= Rank(pos)) ) {
				int prot = 0;
				if ( cb.pionb & mask[pos-7] ) {
					prot++;
				}
				if ( cb.pionb & mask[pos-9] ) {
					prot++;
				}
				if ( prot ) {
					Score += KNIGHT_OUTPOST_BONUS;
				}
				if ( prot == 2 ) {
					Score += KNIGHT_DOUBLE_PROT_BONUS;
				}
		}


		ClearBit( cavaliers, pos );
	}
	cavaliers = cb.cavaliern;
	while( cavaliers ) {
		pos = DernierBit( cavaliers );

		Score -= PositionCavalier[flip[pos]];

		// Bonus for an outpost.
		int col = File(pos)+1;
		if ( (RangeePion[BLANC][col-1] <= Rank(pos)) &&
			 (RangeePion[BLANC][col+1] <= Rank(pos)) ) {
			int prot = 0;
			if ( cb.pionn & mask[pos+7] ) {
				prot++;
			}
			if ( cb.pionn & mask[pos+9] ) {
				prot++;
			}
			if ( prot ) {
				Score -= KNIGHT_OUTPOST_BONUS;
			}
			if ( prot == 2 ) {
				Score -= KNIGHT_DOUBLE_PROT_BONUS;
			}
		}

		ClearBit( cavaliers, pos );
	}

	Bitboard tours = cb.tourb;
	while( tours ) {
		pos = DernierBit( tours );

		if ( RangeePion[BLANC][File(pos)+1] == 0) {
			if (RangeePion[NOIR][File(pos)+1] == 7)
				Score += ROOK_OPEN_FILE_BONUS;
			else
				Score += ROOK_SEMI_OPEN_FILE_BONUS;
		}
		if ( Rank(pos) == 1 ) 
			Score += ROOK_ON_SEVENTH_BONUS;

		ClearBit( tours, pos );
	}
	
	tours = cb.tourn;
	while( tours ) {
		pos = DernierBit( tours );

		if ( RangeePion[NOIR][File(pos)+1] == 7) {
			if (RangeePion[BLANC][File(pos)+1] == 0)
				Score -= ROOK_OPEN_FILE_BONUS;
			else
				Score -= ROOK_SEMI_OPEN_FILE_BONUS;
		}
		if ( Rank(pos) == 6 ) 
			Score -= ROOK_ON_SEVENTH_BONUS;

		ClearBit( tours, pos );
	}

    return Score;
}

SCORE ProtectionRoiBlanc(int ply)
{
	int score;

	int pos = cb.PositionRoiBlanc;

	score = PosRoi[pos];

	// S'il le roi a roque, alors evaluation depend
	// du cote.
	if ( File(pos) < 3 ) {
		score += PionRoiBlanc(1);
		score += PionRoiBlanc(2);
		score += PionRoiBlanc(3) / 2;

	    // Verifier si le fou du fianchetto est present dans
	    // le cas ou le pion b est avance en b3.
	    if ( RangeePion[BLANC][2] == 5 && !(cb.foub && cb.CaseNoire)) {
		    score -= NO_FIANCHETTO_PEN;
			if ( !(cb.foub & mask[B2]) ) {
				score -= NO_FIANCHETTO_PEN;
			}
		}
	}
	else if ( File(pos) > 4 ) {
		score += PionRoiBlanc(8);
		score += PionRoiBlanc(7);
		score += PionRoiBlanc(6) / 2;

		// Verifier si le fou du fianchetto est present dans
		// le cas ou le pion g est avance en g3.
		if ( RangeePion[BLANC][7] && (!cb.foub && cb.CaseBlanche)) {
			score -= NO_FIANCHETTO_PEN;
			if ( !(cb.foub & mask[G2]) ) {
				score -= NO_FIANCHETTO_PEN;
			}
		}
	}

	// Sinon, verifier s'il y a des colonnes
	// ouverte pres du roi.
	else {
		for( int i=File(pos); i <= File(pos)+2; i++ ) {
			if ((RangeePion[BLANC][i] == 0 &&
				 RangeePion[NOIR][i] == 7))
				score -= 10;
		}
	}


	// Plus l'adversaire a de piece, plus c'est enervant.
	score *= cb.ScoreMaterielNoir;
	score /= 3100;

	return score;
}

SCORE PionRoiBlanc( int col )
{
	int score = 0;

	if ( RangeePion[BLANC][col] == 6 ); // Le pion n'a pas bouge.
	else if ( RangeePion[BLANC][col] == 5)
		score -= 10; // Le pion a avance d'une case.
	else if ( RangeePion[BLANC][col] != 0 )
		score -= 20; // Le pion a avance plus d'une case.
	else
		score -= 25; // Pas de pion sur cette colonne.

	if ( RangeePion[NOIR][col] == 7)
		score -= 15; // Pas de pion ennemie.
	else if ( RangeePion[NOIR][col] == 5)
		score -= 10; // Pion ennemie sur la 5 ieme.
	else if ( RangeePion[NOIR][col] == 4)
		score -= 5;  // Pion ennemie sur la 4 ieme.

	return score;
}

SCORE ProtectionRoiNoir(int ply)
{
	int score;

	int pos = cb.PositionRoiNoir;

	score = PosRoi[flip[pos]];

	// S'il le roi a roque, alors evaluation depend
	// du cote.
	if ( File(pos) < 3 ) {
		score += PionRoiNoir(1);
		score += PionRoiNoir(2);
		score += PionRoiNoir(3) / 2;

	    // Verifier si le fou du fianchetto est present dans
	    // le cas ou le pion b est avance en b6.
	    if ( RangeePion[BLANC][2] == 2 && !(cb.foun && cb.CaseBlanche)) {
		    score -= NO_FIANCHETTO_PEN;
			if ( !(cb.foun & mask[B7]) ) {
				score -= NO_FIANCHETTO_PEN;
			}
		}
	}
	else if ( File(pos) > 4 ) {
		score += PionRoiNoir(8);
		score += PionRoiNoir(7);
		score += PionRoiNoir(6) / 2;

	    // Verifier si le fou du fianchetto est present dans
	    // le cas ou le pion g est avance en g6.
	    if ( RangeePion[BLANC][7] == 2 && !(cb.foun && cb.CaseNoire)) {
		    score -= NO_FIANCHETTO_PEN;
			if ( !(cb.foun & mask[G7]) ) {
				score -= NO_FIANCHETTO_PEN;
			}
		}
	}

	// Sinon, verifier s'il y a des colonnes
	// ouverte pres du roi.
	else {
		for( int i=File(pos); i <= File(pos)+2; i++ ) {
			if ((RangeePion[BLANC][i] == 0 &&
				 RangeePion[NOIR][i] == 7))
				score -= 10;
		}
	}

	// Plus l'adversaire a de piece, plus c'est enervant.
	score *= cb.ScoreMaterielBlanc;
	score /= 3100;

	return score;
}


SCORE PionRoiNoir( int col )
{
	int score = 0;

	if ( RangeePion[NOIR][col] == 1 ); // Le pion n'a pas bouge.
	else if ( RangeePion[NOIR][col] == 2)
		score -= 10; // Le pion a avance d'une case.
	else if ( RangeePion[NOIR][col] != 7 )
		score -= 20; // Le pion a avance plus d'une case.
	else
		score -= 25; // Pas de pion sur cette colonne.

	if ( RangeePion[BLANC][col] == 0)
		score -= 15; // Pas de pion ennemie.
	else if ( RangeePion[BLANC][col] == 2)
		score -= 10; // Pion ennemie sur la 5 ieme.
	else if ( RangeePion[BLANC][col] == 3)
		score -= 5;  // Pion ennemie sur la 4 ieme.

	return score;
}
//
// Pour le moment, cette fonction evalue seulement la course lorsque un cote a
// un pion et l'autre en a pas.
//
SCORE EvaluerCoursePionPasses(int wtm)
{
	register Bitboard pions;
	// Verifier si les blanc ont un pion et que les noirs en ont pas.
	if ( cb.TotalPionBlanc > 0 && !cb.TotalPionNoir ) {
		pions = cb.pionb;
		do { 
			int casepion = DernierBit( pions );
			ClearBit( pions, casepion );

			// Premierement, si le pion est en avant du roi, passe au prochain
			// pion.
			if ( Rank( cb.PositionRoiBlanc ) >= Rank( casepion )) 
				continue;	

			// Cas special, un pion tour.
			if ( File( casepion ) == FILEA ) {
				if (( File( cb.PositionRoiBlanc ) == FILEB) &&
					(Distance(cb.PositionRoiBlanc,A8) < Distance(cb.PositionRoiNoir,A8)) )
					return (VALDAME-VALFOU);
				continue;
			}
			else  if (File( casepion ) == FILEH ) {
				if ((File( cb.PositionRoiBlanc ) == FILEG) &&
					(Distance(cb.PositionRoiBlanc,H8) < Distance(cb.PositionRoiNoir,H8)))
					return (VALDAME-VALFOU);
				break;
			}
		
			// Si le roi est 2 cases en avant du pion ou est sur la sixieme
			// rangee et que le roi est blanc est plus proche du pion que le
			// roi noir.
			if (Distance(cb.PositionRoiBlanc, casepion) < Distance(cb.PositionRoiNoir, casepion )) {
				if (Rank(cb.PositionRoiBlanc) < Rank(casepion)-1)
					return (VALDAME-VALFOU);
				if (Rank(cb.PositionRoiBlanc) == RANK6)
					return (VALDAME-VALFOU);
			}
			
			// Si le roi est une case en avant du pion et a l'opposition.
			if ((Rank(cb.PositionRoiBlanc) == Rank(casepion)-1) &&
				GagneOpposition(wtm, cb.PositionRoiBlanc,cb.PositionRoiNoir))
				return (VALDAME-VALFOU);
		} while( pions );
	}
	
	// Verifier si les noirs ont un pion et que les blancs en ont pas.
	if ( cb.TotalPionNoir && !cb.TotalPionBlanc ) {
		pions = cb.pionn;
		do { 
			int casepion = DernierBit( pions );
			ClearBit( pions, casepion );

			// Premierement, si le pion est en avant du roi, passe au prochain
			// pion.
			if ( Rank( cb.PositionRoiNoir ) <= Rank( casepion )) 
				continue;	

			// Cas special, un pion tour.
			if ( File( casepion ) == FILEA ) {
				if (( File( cb.PositionRoiNoir ) == FILEB) &&
					(Distance(cb.PositionRoiNoir,A1) < Distance(cb.PositionRoiBlanc,A1)) )
					return (-(VALDAME-VALFOU));
				continue;
			}
			else  if (File( casepion ) == FILEH ) {
				if ((File( cb.PositionRoiNoir ) == FILEG) &&
					(Distance(cb.PositionRoiNoir,H1) < Distance(cb.PositionRoiBlanc,H1)))
					return (-(VALDAME-VALFOU));
				break;
			}
		
			// Si le roi est 2 cases en avant du pion ou est sur la sixieme
			// rangee.
			if (Distance(cb.PositionRoiNoir, casepion) < Distance(cb.PositionRoiBlanc, casepion )) {
				if (Rank(cb.PositionRoiNoir) < Rank(casepion)+1)
					return (-(VALDAME-VALFOU));
				if (Rank(cb.PositionRoiNoir) == RANK3)
					return (-(VALDAME-VALFOU));
			}

			// Si le roi est une case en avant du pion et a l'opposition.
			if ((Rank(cb.PositionRoiNoir) == Rank(casepion)+1) &&
				GagneOpposition(wtm, cb.PositionRoiBlanc,cb.PositionRoiNoir))
				return (-(VALDAME-VALFOU));
		} while( pions );
	}

	return 0;
}

SCORE EvaluateMaterial(int wtm)
{
	register int score;

	score = cb.ScoreMaterielBlanc - cb.ScoreMaterielNoir;

	//
	// If there is only pawn left and one side as more pawn then the other then penalize
	// the side that have less pawn to prevent exchanging pieces when down a pawn.
	//
	if ( cb.NbWhitePieces + cb.NbBlackPieces == 0 ) {
		if ( cb.TotalPionBlanc > cb.TotalPionNoir ) {
			score += MORE_PAWMS_BONUS;
		}
		else {
			score -= 50;
		}
	}

	//
	// If one side has more piece then the other then give a penality to the side that has more
	// pieces for each piece left to the opponent.
	//
	if ( cb.NbWhitePieces > cb.NbBlackPieces ) {
		score -= cb.NbBlackPieces * EXCHANGE_PEN;
	}
	else if ( cb.NbBlackPieces > cb.NbWhitePieces ) {
		score += cb.NbWhitePieces * EXCHANGE_PEN;
	}

	//
	// If the score is even and one side has more piece then the other
	// give a bonus to the side with more pieces since it is usally
	// better to have more pieces then pawns.
	//
	if ( cb.ScoreMaterielBlanc == cb.ScoreMaterielNoir ) { 
		if ( cb.NbWhitePieces > cb.NbBlackPieces ) {
			score += MORE_PIECES_BONUS;
		}
		else if ( cb.NbBlackPieces > cb.NbWhitePieces ) {
			score -= MORE_PIECES_BONUS;
		}
	}

	return score;
}
