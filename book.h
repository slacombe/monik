//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) March 25, 1998. All right reserved.
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Book.h
// Debut: 8 fevrier 2000.

#ifndef BOOK_H
#define BOOK_H

#define		BOOK_MEM_SIZE_MB		32

struct BookPosition_t 
{
	Bitboard key;
	unsigned short wtm;
	unsigned int freq;
};

int chargerCles( void );
int sauvegarderCles( void );
int loadBook( void );
int book(TChessBoard* cb, int wtm, LINE& ml);
int createStartBook(const char* i_szFilename);

#endif
