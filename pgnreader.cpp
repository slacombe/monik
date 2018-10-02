//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) March 25, 1998. All right reserved.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Book.cpp
// Debut: 7 fevrier 2000.
//
//

#include <stdio.h>
#include <string.h>

#include <iostream>

#include "board.h"
#include "journal.h"
#include "pgnreader.h"

#define	MAXBOOKPLY 20

using namespace std;

bool PgnReader::open(const std::string filename) {
	mPgnFile = fopen(filename.c_str(), "rb");
	if (!mPgnFile) {
		std::cerr << "Cannot open book source: " << filename << std::endl;
        return false;
	}
	mLine = 1;
    return true;
}

void PgnReader::close() {
	fclose(mPgnFile);
}

int PgnReader::readChar() {
    int ch = fgetc(mPgnFile);
    if (ch == '\n') {
        mLine++;
    }
    return ch;
}

bool PgnReader::readCommand() {
	//
	// Lire le fichier jusqu'a ce qu'on
	// trouve un ']'
	int ch = 0;
	while (ch != ']') {
		ch = fgetc(mPgnFile);
	}

	return true;
}

bool PgnReader::readComment() {
	//
	// Lire le fichier jusqu'a ce qu'on
	// trouve un '}'
	int ch = 0;
	while (ch != '}') {
		ch = fgetc(mPgnFile);
	}

	return true;
}

std::string PgnReader::readText() {
	//
	// Lire jusqu'a un ' '
	//
	int ch = 0;
	std::string text;
	while (ch != ' ' && ch != '\r' && ch != '\n' && text.size() < 6) {
		ch = fgetc(mPgnFile);
		if (ch != ' ' && ch != '\r' && ch != '\n') {
			text += ch;
		}
	}

	return text;
}

// Will read on game from a pgn and return it as a ChessGame
ChessGame* PgnReader::readGame() {	

	cb.InitialiseBoard();
	cb.InitialiseBitboard();

	ChessGame* game = NULL;
	
	while (!feof(mPgnFile)) {
		//
		// Lire un caractere.
		//
		int ch = readChar();

		// 
		// Si le caractere est un '[', alors
		// c'est une commande.
		//
		if (ch == '[') {
            if (game && game->nbMoves() > 0) {
                ungetc(ch, mPgnFile);
                return game;
            }
            
			if (!game) {
				game = new ChessGame();
                game->setStartLine(mLine);
			}
			readCommand();
			continue;
		}

		// 
		// Si le caractere est un '{', alors
		// c'est un commentaire.
		//
		if (ch == '{') {
			readComment();
			continue;
		}

		//
		if (ch == '\n') {
			continue;
		}

		if (ch == ' ') {
			continue;
		}

		//
		// Lire jusqu'a un ' ' pour voir
		// si c'est un coup.
		//
		if (strchr("PNBRQKpnrqkabcdefghOo", ch)) {
			ungetc(ch, mPgnFile);
			std::string text = readText();
			
			game->addMove(text);
		}
	}

	return game;
}
