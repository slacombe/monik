/* 
 * File:   UIControllerFactory.cpp
 * Author: Sylvain Lacombe
 * 
 * Created on 15 décembre 2018, 09:57
 */

#include "UIControllerFactory.h"
#include "../protocol/ProtocolFactory.h"

UIControllerFactory::UIControllerFactory() {
}

UIControllerFactory::~UIControllerFactory() {
}

UIController* UIControllerFactory::createUIController() {
	IUIProtocol* protocol = ProtocolFactory().createNoProtocol();
	UIController* uiController = new UIController(protocol);
	return uiController;
}
