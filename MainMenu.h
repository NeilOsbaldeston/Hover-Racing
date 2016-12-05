// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>
#include <vector>
#include "Globals.h"

using namespace tle;


class MainMenu : public Menu
{
private:
	ISprite* title;

	vector<MenuOption*> menuOptions;

	void createTextColBoxes();

public:
	// constructor
	MainMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode = nullptr);

	// destructor
	virtual ~MainMenu();

	int getCarsAI() { return 0; }		// ignore
	int getNumLaps() { return 0; }		// ignore

	string draw(GameStates& gameState);
};