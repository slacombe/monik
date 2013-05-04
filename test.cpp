#include "test.h"
#include "transposition.h"
#include "Board.h"
#include "stdio.h"

void testTransposition()
{
	int ply = 0, depth = 6, wtm = 1, score = 100, alpha = -200, beta = 200, danger = 0;

	TTableTrans* tt = new TTableTrans(32);
	TChessBoard* cb = new TChessBoard();
	tt->Initialise();
	tt->StoreRefutation(*cb, ply, depth, wtm, score, alpha, beta, danger);
	int res = tt->Lookup(*cb, ply, depth, wtm, alpha, beta, danger);
	wtm = 0;
	tt->StoreRefutation(*cb, ply, depth, wtm, score, alpha, beta, danger);

	printf("res = %d\n", res);
	getchar();
}
