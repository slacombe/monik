/* 
 * File:   UIControllerFactory.h
 * Author: Sylvain Lacombe
 *
 * Created on 15 décembre 2018, 09:57
 */

#ifndef UICONTROLLERFACTORY_H
#define UICONTROLLERFACTORY_H

#include "../ui/UIController.h"

class UIControllerFactory {
public:
	UIControllerFactory();
	virtual ~UIControllerFactory();
	UIController* createUIController();
private:

};

#endif /* UICONTROLLERFACTORY_H */

