//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) March 25, 1998. All right reserved.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Book.h
// Debut: 8 fevrier 2000.

#ifndef BOOKREADER_H
#define BOOKREADER_H

#include <sstream>
#include <map>

#include "book.h"
#include "chessgame.h"

using std::string;

class PgnReader {
private:
    int mLine;
	FILE* mPgnFile;
	
    int readChar();
	bool readCommand();
	bool readComment();
	std::string readText();

public:
	bool open(const std::string filenae);
	void close();
	bool isOpen() { return !mPgnFile; }
	bool eof() { return feof(mPgnFile); }
	ChessGame* readGame();
};

#endif
