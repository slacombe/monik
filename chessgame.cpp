/* 
 * File:   ChessGame.cpp
 * Author: Sylvain Lacombe
 * 
 * Created on September 30, 2018, 3:47 PM
 */

#include "chessgame.h"

ChessGame::ChessGame() {
}

ChessGame::ChessGame(const ChessGame& orig) {
}

ChessGame::~ChessGame() {
}

void ChessGame::addMove(const std::string move) {
	mMoves.push_back(move);
}

