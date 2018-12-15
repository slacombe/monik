/* 
 * File:   UIController.h
 * Author: Sylvain Lacombe
 *
 * Created on 15 décembre 2018, 07:54
 */

#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "../protocol/Protocol.h"
#include "../chess.h"

class UIController {
public:
	UIController(IUIProtocol* uiProtocol);
	virtual ~UIController();
	const std::string read();
	void print(const std::string& text);
	void displayTurn(Color color, int moveNb);
private:
	IUIProtocol* uiProtocol() { return _uiProtocol; }
	
	IUIProtocol* _uiProtocol;
};

#endif /* UICONTROLLER_H */

