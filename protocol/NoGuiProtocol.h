/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NoProtocol.h
 * Author: sylvain
 *
 * Created on 15 décembre 2018, 10:07
 */

#ifndef NOGUIPROTOCOL_H
#define NOGUIPROTOCOL_H

#include "NoGuiProtocol.h"
#include "../ui/UIController.h"

class NoGuiProtocol : public IUIProtocol {
public:
	NoGuiProtocol();
	virtual ~NoGuiProtocol();
	virtual void write(const std::string& text);
	const std::string read();
	void displayTurn(Color color, int moveNb);
private:
};

#endif /* NOGUIPROTOCOL_H */

