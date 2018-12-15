/* 
 * File:   UIController.cpp
 * Author: Sylvain Lacombe
 * 
 * Created on 15 décembre 2018, 07:54
 */

#include <iostream>
#include "UIController.h"
#include "../protocol/Protocol.h"

UIController::UIController(IUIProtocol* uiProtocol) : _uiProtocol(uiProtocol) {
}

UIController::~UIController() {
}

const std::string UIController::read() {
	return uiProtocol()->read();
}

	void UIController::print(const std::string& text) {
	uiProtocol()->write(text);
}

void UIController::displayTurn(Color color, int moveNb) {
	uiProtocol()->displayTurn(color, moveNb);
}
