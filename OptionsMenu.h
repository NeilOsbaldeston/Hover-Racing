// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>
#include <vector>
#include "Globals.h"

using namespace tle;


class OptionsMenu : public Menu
{
private:
	vector<MenuOption*> menuOptions;

	void createTextColBoxes();

public:
	// constructor
	OptionsMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode = nullptr);

	// destructor
	virtual ~OptionsMenu();

	// getters
	int getCarsAI() { return 0; }		// ignore
	int getNumLaps() { return 0; }		// ignore

	string draw(GameStates& gameState);
};
