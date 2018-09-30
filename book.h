//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) March 25, 1998. All right reserved.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Book.h
// Debut: 8 fevrier 2000.

#ifndef BOOK_H
#define BOOK_H

#include <sstream>
#include <map>

#include "chessgame.h"

using std::string;

class Book;

extern Book* openingBook;

class Book {
public:
	struct Position_t 
	{
		Bitboard key;
		unsigned short wtm;
		unsigned int freq;
	};

private:
	bool mStartBook;
	std::map<Bitboard, Book::Position_t> mBookTable;

	void initialize();
	void addPosition(TChessBoard& cb, int ply, int wtm);	
	void addGame(const ChessGame& game);
	void save();
	bool loaded() { return !mBookTable.empty(); }
	int lookup(uint64 key, int wtm);
	
public:
	Book();
	
	void load();
	bool create(const std::string filename);
	void createStart(const std::string filename);
	bool inBook(TMoveList& ml, int wtm);
};

#endif

