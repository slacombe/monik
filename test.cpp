#include "test.h"
#include "transposition.h"
#include "board.h"
#include "stdio.h"

void testTransposition()
{
	int ply = 0, depth = 6, wtm = 1, score = 100, alpha = -200, beta = 200, danger = 0;

	createTranspositionTable(32);
	TChessBoard *cb = new TChessBoard();
	initializeTranspositionTable();
	storeRefutation(*cb, ply, depth, wtm, score, alpha, beta, danger);
	uint32 res = lookup(*cb, ply, depth, wtm, alpha, beta, danger);
	wtm = 0;
	storeRefutation(*cb, ply, depth, wtm, score, alpha, beta, danger);

	printf("res = %lu\n", res);
	getchar();
}
