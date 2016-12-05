// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>
#include <vector>
#include "Globals.h"
#include "GameMode.h"

using namespace tle;


class Menu
{
protected:
	I3DEngine* myEngine;

	GameMode* mode;

	IFont* textNormal;	// for menu options
	IFont* textLarge;	// for menu options
	IFont* listFont;	// for track selection

	ISprite* backdrop;
	ISprite* trackBox;

	int halfScreenWidth;
	int halfScreenHeight;
	int mouseX;
	int mouseY;

	int heightAdjustment;	// for switching between fullscreen and windowed
	int widthAdjustment;	// for switching between fullscreen and windowed

	vector<string> tracks;	// the full list of track names
	string track;	// the name of the currently selected track
	int selectedTrack;	// holds the index of the track currently highlighted
	int listPosition;	// if the track list is scrolled down, this number increases

	bool continuePressed;	// triggers the unpause event HoverRacing.cpp
	bool raceAgainPressed;	// reloads the same map with the same settings

	MenuStates* menuState;

	struct MenuOption
	{
		string name;
		int width;
		int height;
	};

	// get the current location of the mouse with relation to the top left of the window/fullscreen
	void updateMousePos();
	// choose to enter a race or go back to the main menu
	string selectMenuOption(GameStates& gameState, vector<MenuOption*> options, IFont* fontNorm, IFont* fontLarge,
		int textPosX, int textPosY, int menuSpacing, EHorizAlignment align = kCentre, unsigned int colour = kBlack);
	// extract the track list by reading the directory contents of our Track folder
	bool getTrackList();
	// search through a selection of tracks
	void scrollTrackList();
	// select from the tracks available (what ever is in the Track folder)
	void selectTrack();

public:
	// constructor
	Menu(I3DEngine* myEngine, GameMode* mode);

	// destructor
	virtual ~Menu();

	// getters //
	
	string getTrack() { return track; };
	virtual int getCarsAI() = 0;
	virtual int getNumLaps() = 0;
	bool getContinuePressed() const { return continuePressed; }
	bool getRaceAgainPressed() const { return raceAgainPressed; }

	// setters //

	void setContinuePressed(bool state) { continuePressed = state; }
	void setRaceAgainPressed(bool state) { raceAgainPressed = state; }

	virtual string draw(GameStates& gameState) = 0;
};