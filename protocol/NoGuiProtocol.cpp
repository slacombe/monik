/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NoProtocol.cpp
 * Author: sylvain
 * 
 * Created on 15 décembre 2018, 10:07
 */

#include<iostream>

#include "Protocol.h"
#include "NoGuiProtocol.h"

NoGuiProtocol::NoGuiProtocol() {
}

NoGuiProtocol::~NoGuiProtocol() {
}

void NoGuiProtocol::write(const std::string& text) {
	std::cout << text << std::endl;
}

const std::string NoGuiProtocol::read() {
	std::string command;
	std::cin >> command;
	return command;
}

void NoGuiProtocol::displayTurn(Color color, int moveNb) {
	const char* colorText = color == Color::white ? "Blanc" : "Noir";
    std::cout << colorText << " (" << moveNb << "): ";
}
