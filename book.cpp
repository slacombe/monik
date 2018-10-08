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

#include "entree.h"
#include "board.h"
#include "make.h"
#include "unmake.h"
#include "book.h"
#include "log.h"
#include "system.h"

BookPosition_t *book_table;
Bitboard book_hash;
int book_size = 0;
int startBook = 0;
int bookLoaded = 0;

int
InitializeBook (void)
{
  //
  // Allouer la memoire pour l'initialisation du livre.
  //
  printf ("Allocating memory...\n");
  book_size = 1024 * 1024 * 2;
  book_table = new BookPosition_t[book_size];
  book_hash = book_size - 1;
  if (!book_table) {
	return -1;
  }
  memset (book_table, 0, book_size * sizeof (BookPosition_t));

  return 0;
}

int
ReadCommand (FILE * fp, char* o_szCommande )
{
  //
  // Lire le fichier jusqu'a ce qu'on
  // trouve un ']'
  int nbChar = 0;
  int ch = 0;
  while (ch != ']') {
	ch = fgetc (fp);
	if ( ch != ']' ) {
		o_szCommande[nbChar] = ch;
		nbChar++;
	}
  }
  o_szCommande[nbChar] = 0;

  return 1;
}

int
ReadComment (FILE * fp)
{
  //
  // Lire le fichier jusqu'a ce qu'on
  // trouve un '}'
  int ch = 0;
  while (ch != '}') {
	ch = fgetc (fp);
  }

  return 1;
}

int
ReadText (FILE * fp, char *o_szText, int i_iMax)
{
  //
  // Lire jusqu'a un ' '
  //
  int ch = 0;
  int nbCar = 0;
  while (ch != ' ' && ch != '\n' && nbCar < i_iMax) {
	ch = fgetc (fp);
	if (ch != ' ' && ch != '\n') {
	  o_szText[nbCar] = ch;
	  nbCar++;
	}
  }
  o_szText[nbCar] = 0;

  return 1;
}


int
UnmakeAll (int i_iPly, int i_iWtm)
{
  //
  // Unmake all the move made so far.
  //
  int wtm = i_iWtm;
  int ply = i_iPly;
  for (int iNoMove = cb.NoCoups - 1; iNoMove >= 0; iNoMove--) {
	UnmakeMove (ply, cb.ListeMove[iNoMove], wtm);
	wtm = !wtm;
  }

  return true;
}

int
ChargerCles (void)
{
  //
  // Charger les cles du livre.
  //
  char bookpath[255];
  MakePath(bookpath, "bookc.bin");
  FILE *fp = fopen (bookpath, "rb");
  if (!fp) {
	return 0;
  }

  for (int i = A8; i <= H1; i++) {
	if ( 1 != fread (&ClePionB[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&ClePionN[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleCavalierB[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleCavalierN[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleFouB[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleFouN[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleTourB[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleTourN[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleDameB[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleDameN[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleRoiB[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
	if ( 1 != fread (&CleRoiN[i], sizeof (Bitboard), 1, fp) ) {
		exit(1);
	}
  }

  fclose (fp);

  return 1;
}

int
SauvegarderCles (void)
{
  //
  // Sauvegarde les cles dans le livre.
  //
  FILE *fp = fopen ("bookc.bin", "wb");
  if (!fp) {
	return 0;
  }

  for (int i = A8; i <= H1; i++) {
	fwrite (&ClePionB[i], sizeof (Bitboard), 1, fp);
	fwrite (&ClePionN[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleCavalierB[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleCavalierN[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleFouB[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleFouN[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleTourB[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleTourN[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleDameB[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleDameN[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleRoiB[i], sizeof (Bitboard), 1, fp);
	fwrite (&CleRoiN[i], sizeof (Bitboard), 1, fp);
  }

  fclose (fp);

  return 1;
}

int
AddToBook (TChessBoard * cb, int ply, int wtm )
{
  // Calculate the key.
  int64 pos = book_hash & cb->CleHachage;

  // Get the board at this position.
  BookPosition_t bookPos = book_table[pos];

  // If the position is not currently use, then
  // add the position to it. If it's already
  // used then, if it is the same position
  // increment the hit, else forget it.
  if (bookPos.key == 0) {
	bookPos.key = cb->CleHachage;
	bookPos.wtm = wtm;
	if ( startBook ) {
		bookPos.freq = 1000;
	}
	else {
		bookPos.freq = 1;
	}

	book_table[pos] = bookPos;
  }
  else if (bookPos.key == cb->CleHachage) {
	if ( bookPos.wtm == wtm ) {
		bookPos.freq++;

		book_table[pos] = bookPos;
	}
  }

  return 1;
}

int KeyCompare( const void* element1, const void* element2 )
{
	BookPosition_t* pElement1 = (BookPosition_t*)element1;
	BookPosition_t* pElement2 = (BookPosition_t*)element2;
	Bitboard key1 = pElement1->key;
	Bitboard key2 = pElement2->key;

	if ( key1 <key2 ) 
		return -1;
	else if ( key2 < key1 )
		return 1;
	else 
		return 0;
}

int
SaveBook (void)
{
  FILE *fp = fopen ("book.bin", "wb");
  if (!fp) {
	fprintf (stderr, "Could not create book file.\n");
	return 0;
  }

  // Sort the table.
  // FIXME: No qsort!!!
  // qsort( book_table, book_size, sizeof( BookPosition_t ), KeyCompare );
  // Go thru the book table and save the positions
  // to the file.
  Bitboard lastkey = 0;
  for (int i = 0; i < book_size; i++) {
	BookPosition_t bookPos = book_table[i];
	assert( lastkey <= bookPos.key );
    lastkey = bookPos.key;
	if (bookPos.key != 0 && bookPos.freq > 5) {
	  fwrite (&bookPos, sizeof (BookPosition_t), 1, fp);
	}
  }

  fclose (fp);

  return 1;
}

int
LoadBook(void)
{
	char bookpath[255];
	MakePath(bookpath, "book.bin");
	FILE *fp = fopen(bookpath, "rb");
	if ( !fp ) {
		fprintf(stderr, "Could not open book file.\n");
		return 0;
	}

	// Load all the book in memory.
	fseek( fp, 0, SEEK_END );
	book_size = ftell( fp ) / sizeof( BookPosition_t );
  	book_table = new BookPosition_t[book_size];

	fseek( fp, 0, SEEK_SET );
	BookPosition_t bookPos;
  	for (int i = 0; i < book_size; i++) {
	  	fread( &bookPos, sizeof (BookPosition_t), 1, fp );
		book_table[i] = bookPos;
	}

	fclose( fp );
	bookLoaded = true;
	return 1;
}

// Binary search in the book.
BookPosition_t*
binLookup( Bitboard key, int wtm, int lo, int hi )
{
	BookPosition_t* book_pos;
	if ( lo > hi ) {
		return 0;
	}

	int mid = (int)floor(float((lo+hi) / 2));
	book_pos = &book_table[mid];
	if ( key == book_pos->key ) {
		return book_pos;
	}
	if ( key > book_pos->key ) {
		return binLookup( key, wtm, mid+1, hi );
	}
	else {
		return binLookup( key, wtm, lo, mid-1 );
	}
}

int
BookLookup( Bitboard key, int wtm )
{
	BookPosition_t* book_pos =  binLookup( key, wtm, 0, book_size );
	if ( book_pos && book_pos->freq > 0 ) {
		if ( book_pos->wtm == wtm ) {
			return book_pos->freq;
		}
	}
	return 0;
}

int 
Book( TChessBoard& cb, int wtm, TMoveList& ml )
{
	if ( !bookLoaded || cb.OutOfBook >= 3 ) {
		return 0;
	}

	//
	// Execute each move and look for the position in the book.
	//
	int i, j;
	for( i=0; i<ml.nbmove; i++ ) {
		MakeMove(1, ml.moves[i], wtm);
		int freq = BookLookup( cb.CleHachage, wtm );
		if ( freq > 0 ) {
			ml.moves[i].Score = freq;
		}
		else {
			ml.moves[i].Score = 0;
		}
		UnmakeMove(1, ml.moves[i], wtm);
	}

	//
	// Transform to percentage.
	//
	int total = 0;
	for( i=0; i<ml.nbmove; i++ ) {
		total += ml.moves[i].Score;
	}
	if ( total > 0 ) {
		for( i=0; i<ml.nbmove; i++ ) {
			int percent = ((ml.moves[i].Score*100) / total);
			ml.moves[i].Score = percent;
		}
	}
	else {
		return false;
	}
	for( i=0; i<ml.nbmove; i++ ) {
		pv[0][i] = ml.moves[i];
	}
	pv_length[0] = ml.nbmove;
	for( i=0; i<pv_length[0]-1; i++ ) 
		for( j=i+1; j<pv_length[0]; j++ )
			if ( pv[0][i].Score < pv[0][j].Score ) {
				TMove temp = pv[0][i];
				pv[0][i] = pv[0][j];
				pv[0][j] = temp;
			}

	int nbMovesInBook = 0;
	for( i=0; i<pv_length[0]; i++ ) {
		if ( pv[0][i].Score > 0 ) {
			nbMovesInBook = i+1;
		}
	}
	pv_length[0] = nbMovesInBook;

	int choosen = -1;
	if ( pv_length[0] >= 1 ) {

		// Take a random number from 0 to 99.
		int randPercent = (int) ((float)100*rand()/(RAND_MAX+1.0));
		// Take the one with the percentage.
		// ie. If 1 = 80%, 2 = 15%, 3 = 5%. Then picking 85 will pick 2.
		gameLog.log( "randPercent = %d\n", randPercent );
		int p = 0;
		for( i=0; i<pv_length[0]; i++ ) {
			p += pv[0][i].Score;
			if ( p >= randPercent ) {
				choosen = i;
				gameLog.log( "Choosen: %d\n", choosen );
				break;
			}
		}
	}

	if ( choosen >= 0 ) {
		pv[1][1] = pv[0][choosen];
		gameLog.log( "In book..." );
		return true;
	}
	else {
		cb.OutOfBook++;
		return false;
	}
}
		
int
CreateStartBook (const char *i_szFilename)
{
  int line = 1;
  InitializeBook ();

  SauvegarderCles ();

  //
  // Try to open the file.
  //
  FILE *fp = fopen (i_szFilename, "rb");
  if (!fp) {
	fprintf (stderr, "Cannot open book source: %s\n", i_szFilename);
	return 1;
  }

  int ply = 0;
  int wtm = 1;
  int illegal = 0;
  while (!feof (fp)) {
	//
	// Lire un caractere.
	//
	int ch = fgetc (fp);

	// 
	// Si le caractere est un '[', alors
	// c'est une commande.
	//
	if (ch == '[') {
	  UnmakeAll (ply, !wtm);
	  wtm = 1;
	  ply = 0;
	  illegal = 0;
	  printf( "." );
	  continue;
	}

    //
	// If the game is illegal, wait until new command. 
 	//
	if ( illegal ) {
		continue;
	}

	// 
	// Si le caractere est un '{', alors
	// c'est un commentaire.
	//
	if (ch == '{') {
	  ReadComment (fp);
	  continue;
	}
	//
	if (ch == '\n') {
	  line++;
	  continue;
	}

	if (ch == ' ') {
	  continue;
	}
	//
	// Lire jusqu'a un ' ' pour voir
	// si c'est un coup.
	//
	if (strchr ("PNBRQKpnrqkabcdefghOo", ch)) {
	  char szText[10];
	  ungetc (ch, fp);
	  ReadText (fp, szText, sizeof (szText) - 1);

	  // 
	  // Convertir le coup dans un format
	  // que Monik comprendra.
	  //
	  TMove move;
	  if (!InputMove(szText, ply, wtm, move)) {
		fprintf (stderr, "Line: %d, Illegal move: %s\n", line, szText);
		// Jump to next game.
		illegal = 1;
		continue;
	  }
	  //
	  // Jouer le coup sur le board.
	  //
	  MakeMove(ply, move, wtm);
	  AddToBook(&cb, ply, wtm);
	  wtm = !wtm;
	  ply++;
	}
  }

  SaveBook ();
  return 1;
}
