/* 
 * File:   Protocol.h
 * Author: Sylvain Lacombe
 *
 * Created on 14 décembre 2018, 18:47
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include "../chess.h"

class IUIProtocol {
public:
	virtual void write(const std::string& text) = 0;
	virtual const std::string read() = 0;
	virtual void displayTurn(Color color, int moveNb) = 0;
};

class IGameProtocol {
public:
	virtual void sendMove(const std::string& move) = 0;
	virtual void sendMove(const TMove& move) = 0;
	virtual void sendResign() = 0;
	virtual void offerDraw() = 0;
};

class IRoomProtocol {
public:
	virtual void tellOponent() = 0;
	virtual void tellOthers() = 0;
};


#endif /* PROTOCOL_H */
