#include <stdlib.h>

#include "test.h"
#include "board.h"
#include "stdio.h"
#include "init.h"

#ifdef TRANSPOSITION
#include "transposition.h"

void testTransposition()
{
	int ply = 0, depth = 6, wtm = 1, score = 100, alpha = -200, beta = 200, danger = 0;

	printf("Creating transposition table\n");
	createTranspositionTable(32);
	printf("Initializing transposition table\n");
	initializeTranspositionTable();

	printf("Creating board\n");
	TChessBoard* cb = (TChessBoard*)malloc(sizeof(TChessBoard));;
	printf("Initializing data\n");
	initialiseData();
	printf("Initializing board\n");
	initialiseBoard(cb);
	printf("Initializing bitboard\n");
	initialiseBitboard(cb);

	printf("Storing refutation\n");
	storeRefutation(cb, ply, depth, wtm, score, alpha, beta, danger);
	printf("Calling lookup\n");
	uint32 res = lookup(cb, ply, depth, wtm, &alpha, &beta, &danger);
	wtm = 0;
	storeRefutation(cb, ply, depth, wtm, score, alpha, beta, danger);

	printf("res = %lu\n", res);
	getchar();
}
#endif

int main(int argc, char **argv) {
	printf("Calling testTransposition\n");
	testTransposition();
}

