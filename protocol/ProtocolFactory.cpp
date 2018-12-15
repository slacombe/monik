/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ProtocolFactory.cpp
 * Author: sylvain
 * 
 * Created on 15 décembre 2018, 10:00
 */

#include "ProtocolFactory.h"
#include "../ui/UIController.h"
#include "../protocol/NoGuiProtocol.h"

ProtocolFactory::ProtocolFactory() {
}

ProtocolFactory::~ProtocolFactory() {
}

IUIProtocol* ProtocolFactory::createNoProtocol() {
	IUIProtocol* uiProtocol = new NoGuiProtocol();
	return uiProtocol;
}
