//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Journal.cpp
// Debut: 2 novembre 1998.
//
//---------------------------------------------------------------------------

#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../utile.h"
#include "log.h"

Log::Log() : mLogGame(false) {
}

void Log::turnOn() {
	mLogGame = true;
}

void Log::startNew() {
	int iDernierNumeroJournal = TrouverDernierJournal();

	std::stringstream s;
	s << "game." << std::setfill('0') << std::setw(3) << iDernierNumeroJournal + 1;
	mFilename = s.str();
}

void Log::log(const TChessBoard& cb) {
	if (!mLogGame)
		return;
	
	char ligne[100], move[6];
	strcpy(ligne, "");
	for (int i = A8; i <= H1; i++) {
		sprintf(move, "%2d ", cb.board[i]);
		strcat(ligne, move);
		if (i % 8 == 7) {
			log(ligne);
			strcpy(ligne, "");
		}
	}
}

void Log::log(const char* i_szMessage, ...) {
	if (!mLogGame)
		return;
	
	std::fstream fs;
	char szTexte[255];

	fs.open(mFilename, std::fstream::out | std::fstream::app);
	if (!fs.is_open())
		return;

	va_list listeParametres;
	va_start(listeParametres, i_szMessage);
	vsprintf(szTexte, i_szMessage, listeParametres);
	fs << szTexte << std::endl;
	va_end(listeParametres);

	fs.close();
}

Log gameLog;