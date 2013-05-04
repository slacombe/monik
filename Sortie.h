//---------------------------------------------------------------------------
#ifndef sortieH
#define sortieH

#include "chess.h"

// Prend un move et genere une chaine lisible.
int SortieMove( TMove& move, char* i_szString );
void Sortie( const char* i_szReponse );
void GetPV( char* o_szContinuation, TMove* ml, int ply );
void postThinking(int profondeur, int score, int elapsed, int nodes, char* continuation);
void GetCurrent( TChessBoard& cb, char* o_szPath, int deep );
void OutBookInfo( char* buffer );
void PrintBoard();
//---------------------------------------------------------------------------
#endif
