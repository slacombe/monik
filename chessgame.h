/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ChessGame.h
 * Author: sylvain
 *
 * Created on September 30, 2018, 3:47 PM
 */

#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <vector>
#include <sstream>

class ChessGame {
public:
	ChessGame();
	ChessGame(const ChessGame& orig);
	virtual ~ChessGame();

    void setStartLine(int line) { mStartLine = line; }
    int getStartLine() const { return mStartLine; }
	void addMove(const std::string move);
	const std::string& getMove(int moveNb) const { return mMoves[moveNb]; };
	int nbMoves() const { return mMoves.size(); };
	
private:
    int mStartLine;
	std::vector<std::string> mPlayer1;
	std::vector<std::string> mPlayer2;
	
	std::vector<std::string> mMoves;
};

#endif /* CHESSGAME_H */

