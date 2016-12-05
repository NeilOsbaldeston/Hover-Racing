// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <vector>
#include <list>
#include "GameMode.h"
#include "Player_HoverCar.h"
#include "AI_HoverCar.h"

using namespace tle;


class Race : public GameMode
{
private:
	I3DEngine* myEngine;

	ISprite* backdropUI;
	ISprite* boostmeter;
	ISprite* racePosBorder;

	Player_HoverCar* hoverCarPlayer;
	vector<AI_HoverCar*> carsAI;

	IMesh* crossMesh;
	list<pair<IModel*, float>> crosses;	// an array of crosses placed to indicate a checkpoint has been passed, 
											// paired with a float holding how long the item has existed

	IMesh* dummyMesh;
	IModel* dummy;	// the camera is attached to this so that 3rd person view does not roll as the car rolls

	IFont* debugFont;
	IFont* countdownFont;
	IFont* uiFont;

	RaceStates* raceState;
	bool showDebugInfo;
	bool startCountDown;
	float raceStartTimer;
	int currentBoost;	// the current boostmeter image

	vector<HoverCar*> racePositions;
	int playerPos;
	vector<pair<HoverCar*, float>> finishPositions;	// as cars cross he finish line, they will be added to this array

	// counts down for 3 seconds in iterations of a second then releases control back to the parent function
	bool raceStartCountDown(float frameTimer);
	// put all cars in a single array and do stage and lap checks on each
	bool checkRacePositions();
	// check to see if the player passes within range of the next check point
	bool stageCheck(HoverCar* car);
	// prints the frame rate info in the top corner of the screen
	void outputDebugInfo(float frameTimer);
	// print race info like speed, lap, stage, hitpoints
	void printRaceInfo();
	// turn the floating point race time into a HH:MM:SS.00 format
	string formatRaceTime(float time);
	// display the boost meter with the correct amount of bars
	void displayBoostMeter();
	// delete the old image and create a new one
	void boostImage(const string& sprite, int imageNum);
	// remove crosses after a time period
	void decayCrosses(float frameTimer);
	// if theres a collision, explode the bomb
	void explodeBombs(float frameTimer);

public:
	// constructor
	Race(I3DEngine* myEngine, RaceStates*& raceState, string track = "", int numAI = 0, int numLaps = 3);

	// destructor
	virtual ~Race();


	RaceCourse& getRaceCourse() { return raceCourse; }

	// game loop
	virtual void start(float frameTimer, GameStates* gameState, MenuStates* menuState);
};