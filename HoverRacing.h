// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "GameMode.h"
#include "Menu.h"

using namespace tle;

class HoverRacing
{
private:
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine;

	// the screen resolution
	int horizontal;
	int vertical;

	// overall state of the game, i.e playing, paused or in the menu
	GameStates gameState;

	// states which menu we are in
	MenuStates menuState;

	// states where if a race is yet to begin, is in progress or finished
	RaceStates* raceState;

	// external (of main game loop) pointer to a race session etc
	GameMode* mode;

	// handle for menu system
	Menu* menu;

	// return value of draw(), used to select between new and edit
	string editorOption;

	// holds the current time taken since the last call, called each frame
	float frameTimer;

public:
	// constructor
	HoverRacing();

	// destructor
	~HoverRacing();

	// game loop
	void runGame();

	// exit if gameState changes to menu
	void exitPlayingMode();

	// release resources
	void cleanUp();
};
