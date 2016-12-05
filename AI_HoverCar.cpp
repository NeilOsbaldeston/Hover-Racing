// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "AI_HoverCar.h"
#include "Extras.h"

using namespace tle;

// constructor
AI_HoverCar::AI_HoverCar(I3DEngine* myEngine, string mesh, RaceCourse* course, float xPos, float yPos, float zPos) :
	HoverCar(myEngine, mesh, course, xPos, yPos, zPos), currentWayPoint(0), distanceSqrdToWayPoint(0.0f), 
	angleToWayPoint(0.0f), allowedAngleToWaypoint(0.0f)
{
	waypointsAI = course->getWaypointsAI();
}

// destructor
AI_HoverCar::~AI_HoverCar()
{
}

// move the AI car to its next waypoint
void AI_HoverCar::moveHoverCar(float frameTimer)
{
	setFrameTime(frameTimer);

	if (currentWayPoint < static_cast<int>(waypointsAI.size()))
	{
		if (!turnToFaceWayPoint())
			resetRoll();

		if (!matchSpeed(AI_MAX_SPEED))
			resetPitch();

		move();
		checkWayPointCollision();
		updateGridPoisition();
	}
}

// when the AI car is within range of its next way point, move to the next way point
void AI_HoverCar::checkWayPointCollision()
{
	float vectorX = waypointsAI[currentWayPoint]->model->GetX() - hoverCar->GetX();
	float vectorZ = waypointsAI[currentWayPoint]->model->GetZ() - hoverCar->GetZ();

	// using distance squared to save on computing the square root as done many times
	distanceSqrdToWayPoint = vectorX * vectorX + vectorZ * vectorZ;

	float waypointRadiusSqrd = AI_WAYPOINT_RADIUS2 * (waypointsAI[currentWayPoint]->scalar * waypointsAI[currentWayPoint]->scalar);

	// find the angle from the centre of the waypoint to the outer edge, from the cars perspective
	float oppAdj = sqrt(waypointRadiusSqrd) / sqrt(distanceSqrdToWayPoint);

	allowedAngleToWaypoint = atan(oppAdj);
	allowedAngleToWaypoint = rads2Deg(allowedAngleToWaypoint);

	if (distanceSqrdToWayPoint <=  waypointRadiusSqrd)
	{
		// collision resolution //

		if (currentWayPoint + 1 < static_cast<int>(waypointsAI.size()))
			currentWayPoint++;
		else
		{
			currentWayPoint = 0;
		}
	}
}

// turn the AI car to face its next way point
bool AI_HoverCar::turnToFaceWayPoint()
{
	// get the vector between the car and waypoint
	float vectorX = waypointsAI[currentWayPoint]->model->GetX() - hoverCar->GetX();
	float vectorZ = waypointsAI[currentWayPoint]->model->GetZ() - hoverCar->GetZ();

	float carMatrix[4][4];
	hoverCar->GetMatrix(&carMatrix[0][0]);

	angleToWayPoint = getAngleXZ(carMatrix[2][0], carMatrix[2][2], vectorX, vectorZ);

	angleToWayPoint = angleToWayPoint * 180.0f / PI;

	// reduces the roll angle as the cars angle to target reduces
	float rollSpeed = ROLL_SPEED - (AI_TURN_DAMPING - angleToWayPoint) + (ROLL_SPEED - AI_TURN_DAMPING);

	if (angleToWayPoint > allowedAngleToWaypoint)
	{
		/*if (angleToWayPoint < AI_TURN_DAMPING)
			turnLeft(rollSpeed);
		else*/
			turnLeft();

		return true;
	}	
	
	if (angleToWayPoint < -allowedAngleToWaypoint)
	{
		/*if (angleToWayPoint < -AI_TURN_DAMPING)
			turnRight(rollSpeed);
		else*/
			turnRight();

		return true;
	}

	return false;
}

// accelerate until speed matches the input, provided within an angle of the waypoint
bool AI_HoverCar::matchSpeed(float speed)
{
	// accelerate if not moving or within the given angle to the target waypoint
	if ((angleToWayPoint < AI_ACCEL_ANGLE_FULL && angleToWayPoint > -AI_ACCEL_ANGLE_FULL) || momentumVelocity < 0.01f)
	{
		// accelerate at full speed if under AI_ACCEL_ANGLE_FULL
		accelerateForward(speed, AI_ACCEL);
		return true;
	}
	else if (angleToWayPoint < AI_ACCEL_ANGLE_HALF && angleToWayPoint > -AI_ACCEL_ANGLE_HALF)
	{
		// upto half max speed at half the velocity if between AI_ACCEL_ANGLE_HALF and AI_ACCEL_ANGLE_FULL
		accelerateForward(speed, AI_ACCEL / 2.0f);
		return true;
	}
	else if (angleToWayPoint < AI_ACCEL_ANGLE_QUARTER && angleToWayPoint > -AI_ACCEL_ANGLE_QUARTER)
	{
		// upto a quarter max speed at a quarter the velocity if between AI_ACCEL_ANGLE_QUARTER and AI_ACCEL_ANGLE_HALF
		accelerateForward(speed, AI_ACCEL / 4.0f);
		return true;
	}
	else if (angleToWayPoint < AI_ACCEL_ANGLE_EIGHTH && angleToWayPoint > -AI_ACCEL_ANGLE_EIGHTH)
	{
		// upto an eighth max speed at an eighth the velocity if between AI_ACCEL_ANGLE_EIGHTH and AI_ACCEL_ANGLE_QUARTER
		accelerateForward(speed, AI_ACCEL / 8.0f);
		return true;
	}
	else
	{
		accelerateForward(speed, AI_ACCEL / 16.0f);
		return true;
	}

	return false;
}