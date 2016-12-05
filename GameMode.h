// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <vector>
#include "MyCamera.h"
#include "RaceCourse.h"

using namespace tle;


class GameMode
{
protected:
	MyCamera myCamera;
	RaceCourse raceCourse;

	bool saving;	// true when prompted to enter a track name when creating / editing a track in the track editor
	string track;
	int numCarsAI;
	int maxLaps;

public:
	/* constructor:
	myEngine	- generic requirement */
	GameMode(I3DEngine* myEngine);

	// virtual destructor so derived destructors are called
	virtual ~GameMode();

	// getters //

	string getTrackName() const { return track; }
	int getNumCarsAI() const { return numCarsAI; }
	int getLaps() const { return maxLaps; }

	// setters //

	void setSaving(bool state) { saving = state; }

	// methods //

	virtual void start(float frameTimer, GameStates* gameState, MenuStates* menuState) = 0;
};