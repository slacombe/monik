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

#include <ostream>
#include <string>
#include <cassert>
#include <string.h>

#include "types.h"

constexpr int ROI = 6;
constexpr int DAME = 5;
constexpr int TOUR = 4;
constexpr int FOU = 3;
constexpr int CAVALIER = 2;
constexpr int PION = 1;

// Definition de quelque type.
constexpr int BOARDSIZE = 64;
constexpr int MAXMOVE = 600;
constexpr int INFINI = 32767;
constexpr int MATE = 30000;
constexpr int MAXPLY = 60;

// Definition pour la librairie d'ouverture.

struct BOOKPOSITION
{
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
	uint32 flag;
};

constexpr int POSITION_PER_SLOT = 600;

constexpr int MAXCHECKEXT = 0;

using Piece = char;
using SCORE = int;

constexpr int VALPION = 100;
constexpr int VALCAVALIER = 320;
constexpr int VALFOU = 330;
constexpr int VALTOUR = 500;
constexpr int VALDAME = 900;
constexpr int VALROI = 2000;

constexpr int BOOKMOVE = 100000;

constexpr int ROQUEROIBLANC = 0x01;
constexpr int ROQUEDAMEBLANC = 0x02;
constexpr int ROQUEROINOIR = 0x04;
constexpr int ROQUEDAMENOIR = 0x08;

constexpr int PAS_EXTENSION = 0;
constexpr int EXTENSION_ECHEC = 1;
constexpr int EXTENSION_RECAPTURE = 2;
constexpr int EXTENSION_PIONPASSE = 3;
constexpr int EXTENSION_PROMOTION = 4;

constexpr int NULLE_REPETITION = 1;
constexpr int NULLE_MANQUE_DE_MATERIEL = 2;
constexpr int NULLE_PAT = 3;
constexpr int NULLE_PAR_ACCORD = 4;
constexpr int NULLE_REGLE_50COUPS = 5;
constexpr int DRAWSCORE = 100;

constexpr int FORKUNCERTAIN = 50;

// Structure pour representer un move.

struct MOVE
{
public:
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
extern const char *nomProgramme;

extern char MonikHome[255];

// benchmark
extern bool benchmark;
extern MOVE moveToFind;

std::ostream& operator<<(std::ostream& os, const MOVE& move);

//---------------------------------------------------------------------------
#endif
