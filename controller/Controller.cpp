/* 
 * File:   Controller.cpp
 * Author: Sylvain Lacombe
 * 
 * Created on 14 décembre 2018, 18:36
 */

#include <iostream>
#include "../console/ConsoleFactory.h"
#include "Controller.h"
#include "../utile.h"
#include "../entree.h"
#include "../engine.h"
#include "../sortie.h"
#include "../transposition.h"
#include "../chess.h"

extern int interrupted;
extern const char* nomProgramme;

Controller::Controller(UIController* uiController) : _uiController(uiController) {
}

Controller::~Controller() {
}

void Controller::init() {
	uiController()->print(nomProgramme);
	uiController()->print("Copyright(C) 2009-2011");
	uiController()->print("Sylvain Lacombe");
}

void Controller::run() {
	char szCommande[255];
	char szReponse[255];
	TSeeker::start();
	do {

		g_bAbort = false;
		fflush(0);
		uiController()->displayTurn(wtm ? Color::white : Color::black, cb.NoCoups/2+1);
		Entree(szCommande);

		bool bSucces = Engine(szCommande, szReponse);

		if (!bSucces) {
			// Soit un coup invalide ou la reponse a une option.
			printf("%s\n", szReponse);
			fflush(stdout);
		} else {
			// On retourne
			if (!g_bModeAnalyse) {
				Sortie(szReponse);
			}
		}
#ifdef TRANSPOSITION
		// Cette fonction peut etre longue a etre executer.
		// C'est pourquoi je l'ai mise apres avoir envoye le coup a WinBoard.
		TableTrans->Initialise();
#endif
	} while (strcmp(szCommande, "quit"));
}

void Controller::interruptReceived() {
	interrupted = 1;
}
