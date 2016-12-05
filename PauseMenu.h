// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>
#include <vector>
#include "Globals.h"
#include "Menu.h"

using namespace tle;


class PauseMenu : public Menu
{
private:
	ISprite* pauseOverlay;

	vector<MenuOption*> menuOptions;

	void createTextColBoxes();

public:
	// constructor
	PauseMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode = nullptr);

	// destructor
	virtual ~PauseMenu();

	// getters
	int getCarsAI() { return 0; }		// ignore
	int getNumLaps() { return 0; }		// ignore

	string draw(GameStates& gameState);
};