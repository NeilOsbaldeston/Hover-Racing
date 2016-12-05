// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Globals.h"
#include "Media.h"
#include "Extras.h"
#include "Bomb.h"


using namespace tle;


// constructor
Bomb::Bomb(I3DEngine* myEngine, IModel* flare) : bomb(flare)
{
	createExplosionParticles(myEngine);

	for (Particle* particle : explosionParticles)
	{
		setExplosionParticlePositions(particle, bomb);
	}
}

// destructor
Bomb::~Bomb()
{
	for (int i = 0; i < BOMB_PARTICLE_COUNT; ++i)
	{
		delete explosionParticles[i];
		explosionParticles[i] = nullptr;
	}
}

// particles //

void Bomb::createExplosionParticles(I3DEngine* myEngine)
{
	particleMesh = myEngine->LoadMesh(PARTICLE);

	for (int i = 0; i < BOMB_PARTICLE_COUNT; ++i)
	{
		explosionParticles.push_back(new Particle{
			particleMesh,
			BOMB_PARTICLE_GRAVITY,
			BOMB_PARTICLE_MIN_SPRAY_ANGLE,
			BOMB_PARTICLE_MAX_SPRAY_ANGLE,
			BOMB_PARTICLE_MIN_CONE_ANGLE,
			BOMB_PARTICLE_MAX_CONE_ANGLE,
			BOMB_PARTICLE_MIN_VEL,
			BOMB_PARTICLE_MAX_VEL,
			FIRE_ORANGE });
	}
}

// set the initial positions at the cars exhaust
void Bomb::setExplosionParticlePositions(Particle* particle, IModel* model)
{
	float matrix[4][4];
	bomb->GetMatrix(&matrix[0][0]);

	particle->getModel()->SetMatrix(&matrix[0][0]);

	particle->setVertParticleVector(model);

	particle->getModel()->Scale(BOMB_PARTICLE_SCALE);
}


// after the particle has been travelling for a period of time, reset it
void Bomb::expireExplosionParticles(Particle* particle, IModel* model)
{
	particle->getModel()->MoveY(-1000.0f);
}