/* 
 * File:   ControllerFactory.cpp
 * Author: Sylvain Lacombe
 * 
 * Created on 14 décembre 2018, 18:37
 */

#include "ControllerFactory.h"
#include "../ui/UIController.h"
#include "../ui/UIControllerFactory.h"

ControllerFactory::ControllerFactory() {
}

ControllerFactory::~ControllerFactory() {
}

Controller* ControllerFactory::createController() {
	UIController* uiController = UIControllerFactory().createUIController();
	Controller* controller = new Controller(uiController);
	controller->init();
	return controller;
}

