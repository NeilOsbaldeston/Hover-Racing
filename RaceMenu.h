// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>
#include <vector>
#include "Globals.h"

using namespace tle;


class RaceMenu : public Menu
{
private:
	ISprite* lapsBox;
	ISprite* opponentsBox;

	int carsAI;
	int numLaps;

	vector<MenuOption*> menuOptions;

	void createTextColBoxes();
	void changeLaps();
	void changeAICars();

public:
	// constructor
	RaceMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode = nullptr);

	// destructor
	virtual ~RaceMenu();

	// getters
	int getCarsAI() { return carsAI; }
	int getNumLaps() { return numLaps; }

	string draw(GameStates& gameState);
};