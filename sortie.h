//---------------------------------------------------------------------------
#ifndef sortieH
#define sortieH

#include "chess.h"
#include "board.h"

// Prend un move et genere une chaine lisible.
int sortieMove(TMove move, char* i_szString );
void sortie(const char* i_szReponse );
void getPV(char* o_szContinuation, TMove* ml, int ply);
void postThinking(int profondeur, int score, int elapsed, int nodes, char* continuation);
void getCurrent(TChessBoard *cb, char* o_szPath, int deep);
void outBookInfo(char* buffer);
void printBoard();
//---------------------------------------------------------------------------
#endif
