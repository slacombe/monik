//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) March 25, 1998. All right reserved.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Book.h
// Debut: 8 fevrier 2000.

#ifndef BOOK_H
#define BOOK_H

#include <string>

#define		BOOK_MEM_SIZE_MB		32

struct BookPosition_t 
{
	Bitboard key;
	unsigned short wtm;
	unsigned int freq;
};

int LoadBook( void );
int Book( TChessBoard& cb, int wtm, TMoveList& ml );
int createBook(const char* filename);
void createStartBook(const char* filename);

#endif
