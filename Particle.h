// Neil Osbaldeston

#pragma once

#include "TL-Engine.h"

using namespace tle;

class Particle
{
private:
	IMesh* myMesh;
	IModel* myModel;
	float moveVector[3];
	float gravity;
	float velocity;
	float lifeTimer;
	bool expired = false;

	int minVertAngle;	// the min spray angle
	int maxVertAngle;	// the max spray angle
	int minDirAngle;	// the min cone angle
	int maxDirAngle;	// the max cone angle
	float xzAngle = 0.0f;
	float xyAngle = 0.0f;
	float zyAngle = 0.0f;
	int minVelocity;
	int maxVelocity;

public:
	// constructor
	Particle(IMesh* mesh, float grav, int minVertAngle, int maxVertAngle,
		int minDirAngle, int maxDirAngle, int minVel, int maxVel, std::string skin = "");

	// destructor
	~Particle();

	// getters //

	IModel* getModel() { return myModel; }
	float getLifeTimer() const { return lifeTimer; }
	bool getExpired() const { return expired; }

	// setters //

	void increaseLifeTimer(float frameTime) { lifeTimer += frameTime; }
	void resetLifeTimer() { lifeTimer = 0.0f; }
	void setExpired(bool state) { expired = state; }

	// set the initial vector upon creation or reset
	void setHorizParticleVector(IModel* model = nullptr);

	void setVertParticleVector(IModel* model = nullptr);

	// move the particle according to its traits
	void manipulateParticle(float frameTime);
};
