// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>
#include <vector>
#include "Globals.h"
#include "Menu.h"

using namespace tle;


class RaceFinishedMenu : public Menu
{
private:
	vector<MenuOption*> menuOptions;

	void createTextColBoxes();

public:
	// constructor
	RaceFinishedMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode = nullptr);

	// destructor
	virtual ~RaceFinishedMenu();

	// getters
	int getCarsAI() { return 0; }		// ignore
	int getNumLaps() { return 0; }		// ignore

	string draw(GameStates& gameState);
};