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

int ChargerCles( void );
int SauvegarderCles( void );
int LoadBook( void );
int Book( TChessBoard& cb, int wtm, TMoveList& ml );
int CreateStartBook( const char* i_szFilename );

#endif
