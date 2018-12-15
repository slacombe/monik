/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ProtocolFactory.h
 * Author: Sylvain Lacombe
 *
 * Created on 15 décembre 2018, 10:00
 */

#ifndef PROTOCOLFACTORY_H
#define PROTOCOLFACTORY_H

#include "../ui/UIController.h"

class ProtocolFactory {
public:
	ProtocolFactory();
	virtual ~ProtocolFactory();
	IUIProtocol* createNoProtocol();
private:

};

#endif /* PROTOCOLFACTORY_H */

