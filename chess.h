//-----------------------------------------------------------------------------
// Projet: SLChess
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Chess.h
// Debut: 4 novembre 1998.
//
//---------------------------------------------------------------------------
#ifndef ChessH
#define ChessH

#include <assert.h>
#include <string.h>

#ifdef _WIN32
typedef unsigned __int64 int64;
#else
typedef unsigned long long int64;
#endif

typedef int64 Bitboard;

#define    ROI             6
#define    DAME            5
#define    TOUR            4
#define    FOU             3
#define    CAVALIER        2
#define    PION            1

// Definition de quelque type.
#define BOARDSIZE   64
#define MAXMOVE     600
#define INFINI      32767
#define MATE        30000
#define MAXPLY      60

// Definition pour la librairie d'ouverture.

struct BOOKPOSITION {
	Bitboard hashkey; // Cle pour identifie la position.
	// 8 premier bit. Qualite de la position.
	// 0000 0001  Tres mauvais coup.
	// 0000 0010  Mauvais coup.
	// 0000 0100  Coup normal.
	// 0000 1000  Bon coup.
	// 0001 0000  Tres bon coup.
	// 0010 0000  Les noirs ont gagne au moins une partie.
	// 0100 0000  Au moins un partie nulle.
	// 1000 0000  Les blancs ont gagne au moins une partie.
	// 24 bit suivant. Nombre de fois joue.
	unsigned int flag;
};

#define  POSITION_PER_SLOT     600

#define MAXCHECKEXT 

typedef char Piece;
typedef int SCORE;

#define        VALPION         100
#define        VALCAVALIER     320
#define        VALFOU          330
#define        VALTOUR         500
#define        VALDAME         900
#define        VALROI          2000

#define   BOOKMOVE  100000

#define		ROQUEROIBLANC   0x01
#define		ROQUEDAMEBLANC  0x02
#define		ROQUEROINOIR    0x04
#define		ROQUEDAMENOIR   0x08

#define		PAS_EXTENSION         0
#define		EXTENSION_ECHEC       1
#define		EXTENSION_RECAPTURE   2
#define		EXTENSION_PIONPASSE   3
#define		EXTENSION_PROMOTION   4

#define		NULLE_REPETITION             1
#define		NULLE_MANQUE_DE_MATERIEL     2
#define		NULLE_PAT                    3
#define		NULLE_PAR_ACCORD             4
#define		NULLE_REGLE_50COUPS          5
#define		DRAWSCORE     100

#define         FORKUNCERTAIN                   50

// Structure pour representer un move.

struct TMove {
	// Une structure de 64 bit.
	// Utile pour pouvoir defaire le coup.
	unsigned From : 6;
	unsigned To : 6;
	int Piece : 4; // La piece qui bouge.
	int Capture : 4; // La piece capture.
	unsigned Roque : 1; // Le roque.
	unsigned EnPassant : 1;
	unsigned Promotion : 4; // 0 ou piece de la promotion.
	int Reserve : 6;
	int Score;
};

class TChessBoard;

// Gestion d'une liste de coup.

class TMoveList {
public:
	int nbmove;
	int currmove;
	TMove moves[MAXMOVE];

	TMoveList() {
		nbmove = 0;
		currmove = 0;
	}

	void Copie(TMoveList& ml) {
		memcpy(moves, ml.moves, ml.nbmove * sizeof ( TMove));
	}

	void Ajoute(TMove& i_move) {
		moves[ nbmove++ ] = i_move;
	}

	void Vide() {
		nbmove = 0;
		currmove = 0;
	}

	TMove& CurrentMove() {
		return moves[currmove];
	}

	void Tri() {
		if (nbmove <= 0)
			return;
		for (int i = 0; i < nbmove - 1; i++)
			for (int j = i + 1; j < nbmove; j++)
				if (moves[i].Score < moves[j].Score) {
					TMove move = moves[i];
					moves[i] = moves[j];
					moves[j] = move;
				}
	}

	void ChoisiMove(int ply, int wtm);
	void ChoisiNonQuiet(int ply, int wtm);
};


#define pion       1
#define cavalier   2
#define fou        3
#define tour       4
#define dame       5
#define roi        6

enum {
	A8, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1
};

// Liste le dernier bit de toutes les combinaisons possibles
// pour un entier de 16 bits. Donc 65536 possibilitees.
extern unsigned char dernier_bits[65536];

extern int ValeurPiece[];

extern Bitboard ClePionB[BOARDSIZE];
extern Bitboard ClePionN[BOARDSIZE];
extern Bitboard CleCavalierB[BOARDSIZE];
extern Bitboard CleCavalierN[BOARDSIZE];
extern Bitboard CleFouB[BOARDSIZE];
extern Bitboard CleFouN[BOARDSIZE];
extern Bitboard CleTourB[BOARDSIZE];
extern Bitboard CleTourN[BOARDSIZE];
extern Bitboard CleDameB[BOARDSIZE];
extern Bitboard CleDameN[BOARDSIZE];
extern Bitboard CleRoiB[BOARDSIZE];
extern Bitboard CleRoiN[BOARDSIZE];

extern bool xboard;
extern bool g_bAbort;
extern bool g_bModeAnalyse;
extern const char* nomProgramme;

extern int g_iTranspositionHit, g_iTranspositionCollision;
extern int g_iRefutation;
extern char MonikHome[255];

//---------------------------------------------------------------------------
#endif
