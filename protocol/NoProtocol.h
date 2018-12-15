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

#ifndef NOPROTOCOL_H
#define NOPROTOCOL_H

#include "NoProtocol.h"
#include "../ui/UIController.h"

class NoProtocol : public IUIProtocol {
public:
	NoProtocol();
	virtual ~NoProtocol();
	virtual void write(const std::string& text);
	const std::string& read();
	void displayTurn(Color color, int moveNb);
private:
};

#endif /* NOPROTOCOL_H */

