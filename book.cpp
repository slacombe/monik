//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) March 25, 1998. All right reserved.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Book.cpp
// Debut: 7 fevrier 2000.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string>
#include <iostream>

#include "entree.h"
#include "board.h"
#include "make.h"
#include "unmake.h"
#include "book.h"
#include "journal.h"
#include "pgnreader.h"

#define	MAXBOOKPLY 20

using namespace std;

Book* openingBook;

Book::Book() : mStartBook(false) {}

void Book::initialize() {
	mBookTable.clear();
}

void Book::addPosition(TChessBoard& cb, int ply, int wtm) {
	// Calculate the key.
	uint64 pos = cb.CleHachage;

	// Get the board at this position.
	Book::Position_t bookPos = mBookTable[pos];

	// If the position is not currently use, then
	// add the position to it. If it's already
	// used then, if it is the same position
	// increment the hit, else forget it.
	if (bookPos.key == 0) {
		bookPos.key = cb.CleHachage;
		bookPos.wtm = wtm;
		if (mStartBook) {
			bookPos.freq = 1000;
		} else {
			bookPos.freq = 1;
		}

		mBookTable[pos] = bookPos;
	} else if (bookPos.key == cb.CleHachage) {
		if (bookPos.wtm == wtm) {
			bookPos.freq++;

			mBookTable[pos] = bookPos;
		}
	}
}

void Book::addGame(const ChessGame& game) {
	int wtm = 1;
	int ply = 0;
	for(int i=0; i<game.nbMoves() && i<20; i++) {
		std::string move = game.getMove(i);
		
		TMove engineMove;
		if (InputMove(move.c_str(), ply, wtm, engineMove)) {
            MakeMove(ply, engineMove, wtm);
			addPosition(cb, ply, wtm);
            wtm = !wtm;
            ply++;
		} else {
            std::cerr << endl << "Game starting at line: " << game.getStartLine() << ", Illegal move: " << move << std::endl;
			break;
		}
	}
}

void Book::save() {
	FILE *fp = fopen("book.bin", "wb");
	if (!fp) {
		std::cerr << "Could not create book file." << std::endl;
		return;
	}

	// Go thru the book table and save the positions
	// to the file.
	for(std::map<uint64, Book::Position_t>::iterator it = mBookTable.begin();
		it != mBookTable.end(); it++) {
		if (it->second.freq > 5) {
			fwrite(&it->second, sizeof(Book::Position_t), 1, fp);
		}
	}
	
	fclose(fp);
}

void Book::load() {
	FILE *fp = fopen("book.bin", "rb");
	if (!fp) {
		std::cerr << "Could not open book file." << std::endl;
		return;
	}

	// Load all the book in memory.
	mBookTable.clear();
	Book::Position_t bookPos;
	while(!feof(fp)) {
		fread(&bookPos, sizeof(Book::Position_t), 1, fp);
		mBookTable[bookPos.key] = bookPos;
	}

	fclose(fp);
    
    cout << "Opening Book with " << mBookTable.size() << " positions" << endl;
}

int Book::lookup(uint64 key, int wtm) {
	if (mBookTable.count(key) == 1) {
		Book::Position_t& pos = mBookTable[key];
		if (pos.freq > 0) {
			if (pos.wtm == wtm) {
				return pos.freq;
			}
		}
	}
	return 0;
}

bool Book::inBook(TMoveList& ml, int wtm) {
	if (!loaded() || cb.OutOfBook >= 3) {
		return 0;
	}

	//
	// Execute each move and look for the position in the book.
	//
	int i, j;
	for (i = 0; i < ml.nbmove; i++) {
		MakeMove(1, ml.moves[i], wtm);
		int freq = lookup(cb.CleHachage, wtm);
		if (freq > 0) {
			ml.moves[i].Score = freq;
		} else {
			ml.moves[i].Score = 0;
		}
		UnmakeMove(1, ml.moves[i], wtm);
	}

	//
	// Transform to percentage.
	//
	int total = 0;
	for (i = 0; i < ml.nbmove; i++) {
		total += ml.moves[i].Score;
	}
	if (total > 0) {
		for (i = 0; i < ml.nbmove; i++) {
			int percent = ((ml.moves[i].Score * 100) / total);
			ml.moves[i].Score = percent;
		}
	} else {
		return false;
	}
	for (i = 0; i < ml.nbmove; i++) {
		pv[0][i] = ml.moves[i];
	}
	pv_length[0] = ml.nbmove;
	for (i = 0; i < pv_length[0] - 1; i++)
		for (j = i + 1; j < pv_length[0]; j++)
			if (pv[0][i].Score < pv[0][j].Score) {
				TMove temp = pv[0][i];
				pv[0][i] = pv[0][j];
				pv[0][j] = temp;
			}

	int nbMovesInBook = 0;
	for (i = 0; i < pv_length[0]; i++) {
		if (pv[0][i].Score > 0) {
			nbMovesInBook = i + 1;
		}
	}
	pv_length[0] = nbMovesInBook;

	int choosen = -1;
	if (pv_length[0] >= 1) {

		// Take a random number from 0 to 99.
		int randPercent = (int) ((float) 100 * rand() / (RAND_MAX + 1.0));
		// Take the one with the percentage.
		// ie. If 1 = 80%, 2 = 15%, 3 = 5%. Then picking 85 will pick 2.
		journal.Log("randPercent = %d\n", randPercent);
		int p = 0;
		for (i = 0; i < pv_length[0]; i++) {
			p += pv[0][i].Score;
			if (p >= randPercent) {
				choosen = i;
				journal.Log("Choosen: %d\n", choosen);
				break;
			}
		}
	}

	if (choosen >= 0) {
		pv[1][1] = pv[0][choosen];
		journal.Log("In book...");
		return true;
	} else {
		cb.OutOfBook++;
		return false;
	}
}

bool Book::create(const std::string filename) {
	int line = 1;
	
	//
	// Try to open the file.
	//
	PgnReader reader;
    if (!reader.open(filename)) {
        return false;
    }

	int ply = 0;
	int wtm = 1;

    int nbGames = 0;
	ChessGame* game;
	while((game = reader.readGame())) {
		addGame(*game);
        nbGames++;
        delete game;
        if (nbGames % 100 == 1) {
            cout << '.';
        }
        if (nbGames % 5000 == 0) {
            cout << " " << nbGames << endl;
        }
	}

    cout << endl;
    cout << "Read " << nbGames << " games" << endl;
	save();
    load();
	
	return true;
}

void Book::createStart(const std::string filename) {
	mStartBook = true;
	create(filename);
}
