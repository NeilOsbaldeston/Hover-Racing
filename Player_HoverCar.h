// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <string>
#include "Media.h"
#include "HoverCar.h"
#include "RaceCourse.h"

using namespace tle;

class Player_HoverCar : public HoverCar
{
private:
	Player_HoverCar() = delete;	// make sure a default constructor is never created

	float boostHeat;	// how long the boost is held for
	bool overHeat;	// if the booster is over used it will over-heat and shut down the thruster
	float overHeatTimer;	// how long the boost has been deactivated for
	bool boosterActive;	// if overheated, becomes inactive for a period
	vector<Particle*> boostParticles;

	CarState playerCarAcel;
	CarState playerCarTurn;

	void createBoostParticles();

public:
	// constructor
	Player_HoverCar(I3DEngine* myEngine, string mesh, RaceCourse* course, float xPos = DEFAULT_START_XPOS, 
		float yPos = DEFAULT_START_YPOS, float zPos = DEFAULT_START_ZPOS);

	// destructor
	virtual ~Player_HoverCar();

	// getters //

	float getBoostHeat() const { return boostHeat; }
	vector<Particle*> getBoostParticles() { return boostParticles; }
	bool getBoosterActive() const { return boosterActive; }

	// setters //

	void applyBoost();
	void setBoostParticlePositions(Particle* particle, IModel* model);
	void expireBoostParticles(Particle* particle, IModel* model = nullptr);

	// when the boost is not been used
	void coolDown();

	// determines movement from key presses, moves the car and displays debug info
	void moveHoverCar(IFont* debugFont, IModel* dummy, float frameTimer, bool showDebugInfo);
};