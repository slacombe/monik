//-----------------------------------------------------------------------------
// Projet: SLChess
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: TJournal.h
// Debut: 2 novembre 1998.
// Description: Permet d'ecrire dans un log.
//
//---------------------------------------------------------------------------
#ifndef LogH
#define LogH

#undef min
#include <fstream>
#include "../board.h"

using namespace std;

class Log : fstream {
	string mFilename;
	bool mLogGame;
public:
	Log();
	void turnOn();
	void startNew();
	void log(const char* i_szMessage, ...);
	void log(const TChessBoard& cb);
};

extern Log gameLog;

//---------------------------------------------------------------------------
#endif

