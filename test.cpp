#include <iostream>

#include "test.h"
#include "board.h"
#include "stdio.h"
#include "init.h"

#include "transposition.h"

#ifdef TRANSPOSITION

using namespace std;

void testTransposition()
{
	int ply = 1, depth = 6, wtm = 1, score = 100, alpha = -200, beta = 200, danger = 1;

	cout << "Creating transposition table" << endl;
	createTranspositionTable(32*1024*1024);
	cout << "Initializing transposition table" << endl;

	cout << "Creating board" << endl;
	TChessBoard* cb = (TChessBoard*)malloc(sizeof(TChessBoard));;
	cout << "Initializing data" << endl;
	initialiseData();
	cout << "Initializing board" << endl;
	initialiseBoard(cb);
	cout << "Initializing bitboard" << endl;
	initialiseBitboard(cb);

	cout << "Storing refutation" << endl;
	storeRefutation(cb, ply, depth, wtm, score, danger);
	cout << "Calling lookup" << endl;
	uint32 res = lookup(cb, ply, depth, wtm, &alpha, &beta, &danger);
	cout << "danger = " << danger << endl;
	cout << "alpha = " << alpha << endl;
	cout << "beta = " << beta << endl;
	cout << "res = " << res << endl;

	TMove move;
	move.From = 62;
	move.To = 45;
	move.Piece = CAVALIER;
	wtm = 0; alpha = 100; beta = 101; danger = 0;
	pv[1][1] = move;
	storeBest(cb, ply, 2, wtm, alpha, 0, danger);
	uint32 resbest = lookup(cb, ply, 2, wtm, &alpha, &beta, &danger);
	cout << "alpha = " << alpha << endl;
	cout << "beta = " << beta << beta << endl;
	cout << "danger = " << danger << endl;
	cout << cb->HashMove[ply] << endl;
	cout << "resbest = " << resbest << endl;
	getchar();
}
#endif
