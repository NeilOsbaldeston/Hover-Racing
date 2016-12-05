// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <vector>
#include "Particle.h"

using namespace tle;

class Bomb
{
private:
	IModel* bomb;
	IMesh* particleMesh;
	std::vector<Particle*> explosionParticles;

	bool activated = false;

	void createExplosionParticles(I3DEngine* myEngine);

public:
	// constructor
	Bomb(I3DEngine* myEngine, IModel* flare);

	// destructor
	~Bomb();

	// getters //

	IModel* getModel() { return bomb; }
	bool getActivated() const { return activated; }
	std::vector<Particle*>& getParticles() { return explosionParticles; }

	// setters //

	void setActivated(bool state) { activated = state; }


	// set the initial positions at the cars exhaust
	void setExplosionParticlePositions(Particle* particle, IModel* model);

	// after the particle has been travelling for a period of time, reset it
	void expireExplosionParticles(Particle* particle, IModel* model);

};