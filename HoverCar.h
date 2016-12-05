// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <string>
#include "Media.h"
#include "Globals.h"
#include "RaceCourse.h"
#include "Particle.h"

// forward declarations
class Player_HoverCar;
class AI_HoverCar;

using namespace tle;

class HoverCar
{
private:
	// make sure a default constructor is never created
	HoverCar() = delete;
	HoverCar(const HoverCar& copy) = delete;

protected:
	I3DEngine* myEngine;
	IMesh* hoverCarMesh;
	IModel* hoverCar;
	vector<map<int, vector<Sphere*>>> checkPointObjects;
	vector<map<int, vector<Box*>>> allBoxOjects;
	vector<map<int, vector<Sphere*>>> allSphereOjects;
	list<Bomb*> bombs;

	Player_HoverCar* hoverCarPlayer;
	vector<AI_HoverCar*> carsAI;

	string name;	// the name of the driver

	float frameTime;

	float facingVelocity;	// speed travelling in facing direction
	float momentumVelocity;	// the speed the car is moving in any direction
	float speed;	// altered by acceleration
	float fallSpeed;	// affected by gravity
	float maxFallSpeed;
	float currentMaxFallSpeed;
	float fallSpeedTimer;
	float turnSpeed;	// rotation per frame in degrees
	float turnAngle;	// the angle the car is facing with relation to north
	float rollAmount;	// angle in degrees
	float pitchAmount;	// angle in degrees
	float driftVector[3];	// current direction vector of drift
	int grid;	// same reference as held by the objects
	int gridX;	// grid along the X axis starting at 0 in the bottom left
	int gridZ;	// grid along the Z axis starting at 0 in the bottom left
	float collisionAngle;	// positive or negative
	float bounceControl;	// collisions
	float hitPoints;
	bool collisionRotation;
	float rotationVelocity;

	// to store the direction of the last collision to know how to react to the next
	enum class CollisionDirection
	{
		NONE,
		LEFT_SIDE_MINUS,
		LEFT_SIDE_PLUS,
		RIGHT_SIDE_MINUS,
		RIGHT_SIDE_PLUS,
	};
	CollisionDirection lastCol = CollisionDirection::NONE;

	// race details
	int currentLap;
	int currentStage;
	float checkPointDistance2;	// distance squared to the next check point
	bool finished;

	// particles effects
	IMesh* particleMesh;
	vector<Particle*> engineParticles;

	void createEngineParticles();

	// constructor
	HoverCar(I3DEngine* myEngine, string mesh, RaceCourse* raceCourse, float xPos, float yPos, float zPos);

public:
	virtual ~HoverCar();

	void displayInfo(IFont* debugFont);

	// getters //

	string getDriverName() const { return name; }
	IModel* getModel() const { return hoverCar; }
	float getSpeed() const { return facingVelocity; }
	int getHitPoints() const { return static_cast<int>(hitPoints); }
	int getCurrentLap() const { return currentLap; }
	int getCurrentStage() const { return currentStage; }
	float getCheckPointDistance2() const { return checkPointDistance2; }
	bool getFinished() const { return finished; }

	// setters //

	void setDriverName(string newName) { name = newName; }
	void setCurrentLap(int newLap) { currentLap = newLap; }
	void setCurrentStage(int newStage) { currentStage = newStage; }
	void setCheckPointDistance2(float distance2) { checkPointDistance2 = distance2; }
	void setFinished(bool state) { finished = state; }

	void setCarLists(Player_HoverCar*& hoverCarPlayer, vector<AI_HoverCar*>& carsAI);
	void setFrameTime(float time);

	void move();

	void turnLeft(float alpha = ROLL_SPEED);
	void turnRight(float alpha = ROLL_SPEED);
	void resetRoll();
	void collisionTurn();

	void accelerateForward(float maxSpeed = FORWARD_MAX_SPEED, float acceleration = FORWARD_ACCELERATION, float alpha = PITCH_SPEED);
	void accelerateBackward(float maxSpeed = BACKWARD_MAX_SPEED, float acceleration = BACKWARD_ACCELERATION, float alpha = PITCH_SPEED);
	void resetPitch();

	// public particle methods //

	void setEngineParticlePositions(Particle* particle, IModel* model);
	vector<Particle*> getEngineParticles() { return engineParticles; }
	void expireEngineParticles(Particle* particle, IModel* model = nullptr);


protected:
	void updateDummy(IModel* dummy);

	void findFacingSpeed();
	void applyDrag();

	void getDriftVector();
	void drift();
	void hover();

	void updateGridPoisition();

	template <class T>
	bool checkCollisionGrids(vector<map<int, vector<T*>>>& objects,  bool (HoverCar::*collisionCheck)(T*&, float, float));

	bool sphereToBoxCollisions(Box*& object, float objX, float objZ);
	bool sphereToCheckPointCollisions(Sphere*& object, float objX = 0.0f, float objZ = 0.0f);
	bool checkBoundaryCollision();
	bool carToCarCollisions(Player_HoverCar* hoverCarPlayer, vector<AI_HoverCar*> carsAI);
	void boxCollisionResolution(float carX, float carZ, float objectLeft, float objectRight, float objectFront, float objectBack);
	bool sphereToSphereCollisions(Sphere*& object, float objX, float objZ);
};

/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

// passing a member function pointer so that the same function can be used to check different types
template <class T>
bool HoverCar::checkCollisionGrids(vector<map<int, vector<T*>>>& objects, bool (HoverCar::*collisionCheck)(T*&,float, float))
{
	float playerX = hoverCar->GetX();
	float playerZ = hoverCar->GetZ();

	float objX = 0.0f;
	float objZ = 0.0f;
	float vectorX = 0.0f;
	float vectorZ = 0.0f;
	float distance2 = 0.0f;	// distance squared

	// go through each of the object types
	for (auto objectType = objects.begin(); objectType != objects.end(); ++objectType)
	{
		// select only the grids directly around the players grid and the players grid
		for (int gridRows = grid - MAP_GRID_SIZE; gridRows <= grid + MAP_GRID_SIZE; gridRows += MAP_GRID_SIZE)
		{
			// check were still in bounds along the column
			if (gridRows < MAP_GRID_SIZE * MAP_GRID_SIZE && gridRows >= 0)
			{
				for (int gridCols = gridRows - 1; gridCols <= gridRows + 1; ++gridCols)
				{
					// check were still in bounds along the row
					if (gridCols % MAP_GRID_SIZE > 0 && gridCols % MAP_GRID_SIZE < MAP_GRID_SIZE)
					{
						// iterate though all the objects in the grid
						for (auto object = objectType->at(gridCols).begin(); object != objectType->at(gridCols).end(); ++object)
						{
							// check collision //

							float objX = (*object)->model->GetX();
							float objZ = (*object)->model->GetZ();

							if ((this->*collisionCheck)(*object, objX, objZ))
								return true;
						}
					}
				}
			}
		}
	}
	return false;
}