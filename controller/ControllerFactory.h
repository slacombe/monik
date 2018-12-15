/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ControllerFactory.h
 * Author: sylvain
 *
 * Created on 14 décembre 2018, 18:37
 */

#ifndef CONTROLLERFACTORY_H
#define CONTROLLERFACTORY_H

#include "Controller.h"

class ControllerFactory {
public:
	ControllerFactory();
	virtual ~ControllerFactory();
	Controller* createController();
private:

};

#endif /* CONTROLLERFACTORY_H */
