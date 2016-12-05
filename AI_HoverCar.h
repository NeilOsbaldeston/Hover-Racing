// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <string>
#include "Media.h"
#include "HoverCar.h"
#include "Globals.h"

using namespace tle;

class AI_HoverCar : public HoverCar
{
private:
	AI_HoverCar() = delete;	// make sure a default constructor is never created

	vector<Sphere*> waypointsAI;
	int currentWayPoint;
	float distanceSqrdToWayPoint;
	float angleToWayPoint;
	float allowedAngleToWaypoint;

public:
	// constructor
	AI_HoverCar(I3DEngine* myEngine, string mesh, RaceCourse* course, float xPos = DEFAULT_START_XPOS, 
		float yPos = DEFAULT_START_XPOS - 20.0f, float zPos = DEFAULT_START_XPOS - 20.0f);

	// destructor
	virtual ~AI_HoverCar();

	// move the AI car to its next waypoint
	void moveHoverCar(float frameTimer);

	// when the AI car is within range of its next way point, move to the next way point
	void checkWayPointCollision();

	// turn the AI car to face its next way point
	bool turnToFaceWayPoint();

	// accelerate until speed matches the input
	bool matchSpeed(float speed = 0.0f);
};