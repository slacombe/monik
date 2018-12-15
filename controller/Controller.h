/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Controller.h
 * Author: sylvain
 *
 * Created on 14 décembre 2018, 18:36
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../ui/UIController.h"

class Controller {
public:
	Controller(UIController* uiController);
	virtual ~Controller();
	
	void init();
	void run();
	void interruptReceived();
private:
	UIController* uiController() { return _uiController; };
	
	UIController* _uiController;
};

#endif /* CONTROLLER_H */
