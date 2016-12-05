// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Player_HoverCar.h"
#include "Globals.h"
#include "Extras.h"

using namespace tle;

// constructor
Player_HoverCar::Player_HoverCar(I3DEngine* myEngine, string mesh, RaceCourse* course, float xPos, float yPos, float zPos) :
	HoverCar(myEngine, mesh, course, xPos, yPos, zPos), boostHeat(0.0f), overHeat(false), overHeatTimer(0.0f), boosterActive(true),
	playerCarAcel(CarState::NONE), playerCarTurn(CarState::NONE)
{
	createBoostParticles();
}

// destructor
Player_HoverCar::~Player_HoverCar()
{
}

void Player_HoverCar::applyBoost()
{
	if (myEngine->KeyHeld(BOOST))
	{
		// record how long the charge has been active
		boostHeat += frameTime;

		// if not over-heating
		if (boostHeat < BOOST_HEAT_LIMIT)
		{
			// pitch the car
			if (pitchAmount - BOOST_MAX_PITCH * BOOST_ACCELERATION * frameTime >= -BOOST_MAX_PITCH)
			{
				pitchAmount -= BOOST_MAX_PITCH * BOOST_ACCELERATION * frameTime;
				hoverCar->RotateLocalX(-BOOST_MAX_PITCH * BOOST_ACCELERATION * frameTime);
			}

			// speed up
			if (facingVelocity < BOOST_MAX_SPEED)
			{
				if (pitchAmount >= 0.0f)
					speed += BOOST_ACCELERATION * (pitchAmount / BOOST_MAX_PITCH) * frameTime;
				else
					speed += BOOST_ACCELERATION * (-pitchAmount / BOOST_MAX_PITCH) * frameTime;
			}
		}
		else
		{
			// booster fails, rapid decceleration....
			boosterActive = false;
			overHeat = true;
		}
	}
	else
	{
		coolDown();

		if (pitchAmount < -MAX_BACKWARD_PITCH)
			resetPitch();
	}
}

// when the boost is not been used
void Player_HoverCar::coolDown()
{
	if (boostHeat > 0.0f)
	{
		// the booster cools down slower than than you use it
		boostHeat -= frameTime / BOOST_RECHARGE_SPEED;
	}
}


// particles //

void Player_HoverCar::createBoostParticles()
{
	particleMesh = myEngine->LoadMesh(PARTICLE);

	for (int i = 0; i < ENGINE_PARTICLE_COUNT; ++i)
	{
		boostParticles.push_back(new Particle{
			particleMesh,
			BOOST_PARTICLE_GRAVITY,
			BOOST_PARTICLE_MIN_SPRAY_ANGLE,
			BOOST_PARTICLE_MAX_SPRAY_ANGLE,
			BOOST_PARTICLE_MIN_CONE_ANGLE,
			BOOST_PARTICLE_MAX_CONE_ANGLE,
			BOOST_PARTICLE_MIN_VEL,
			BOOST_PARTICLE_MAX_VEL,
			BOOST_BLUE });
	}
}

// set the initial positions at the cars exhaust
void Player_HoverCar::setBoostParticlePositions(Particle* particle, IModel* model)
{
	float matrix[4][4];
	hoverCar->GetMatrix(&matrix[0][0]);

	particle->getModel()->SetMatrix(&matrix[0][0]);

	// set a random location within the thruster port
	float radius = random(BOOST_PARTICLE_STARTING_MIN_RADIUS, BOOST_PARTICLE_STARTING_MAX_RADIUS);
	float moveX = random(static_cast<int>(-radius), static_cast<int>(radius)) / 100.0f;
	float moveY = sqrtf((radius / 100.0f) * (radius / 100.0f) - moveX * moveX);
	float moveZ = random(-BOOST_PARTICLE_Z_OFFSET, BOOST_PARTICLE_Z_OFFSET) / 100.0f;

	// find out if we want a positive Y or negative Y
	int sign = static_cast<int>(random(0, 1) + 0.5f);

	// get the particle to the exhaust
	particle->getModel()->MoveLocalY(2.7f);
	particle->getModel()->MoveLocalZ(-7.5f);

	// position the particle randomly in the exhaust
	if (sign == 0)
		particle->getModel()->MoveLocalY(moveY);
	else
		particle->getModel()->MoveLocalY(-moveY);

	particle->getModel()->MoveLocalX(moveX);
	particle->getModel()->MoveLocalZ(moveZ);

	// find the particles vector by looking at the facing direction 
	// of the car and the randomised direction of the particle
	particle->setHorizParticleVector(model);

	particle->getModel()->Scale(BOOST_PARTICLE_SCALE);
}

// after the particle has been travelling for a period of time, reset it
void Player_HoverCar::expireBoostParticles(Particle* particle, IModel* model)
{
	if (particle->getLifeTimer() > BOOST_PARTICLE_LIFETIME)
	{
		setBoostParticlePositions(particle, model);

		// set lifeTimer back to zero
		particle->resetLifeTimer();
	}
}

// movement //

// determines movement from key presses, moves the car and displays debug info
void Player_HoverCar::moveHoverCar(IFont* debugFont, IModel* dummy, float frameTimer, bool showDebugInfo)
{
	setFrameTime(frameTimer);
	playerCarAcel = CarState::NONE;
	playerCarTurn = CarState::NONE;

	if (!overHeat)
	{
		if (myEngine->KeyHeld(FORWARD))
		{
			accelerateForward();
			playerCarAcel = CarState::ACCELERATING_FORWARD;
		}

		if (myEngine->KeyHeld(BACKWARD))
		{
			accelerateBackward();
			playerCarAcel = CarState::ACCELERATING_BACKWARD;
		}
	}
	else
	{
		overHeatTimer += frameTime;

		if (overHeatTimer >= DECELERATION_PERIOD)
			overHeat = false;

		if (boostHeat > 0.0f)
		{
			// the booster cools down very slowly
			boostHeat -= frameTime / BOOST_RECHARGE_OVERHEAT;
		}
	}

	// if the booster is active we can apply boost else we count up to the alotted wait time and reducing heat at a slow rate
	if (boosterActive)
	{
		if (playerCarAcel == CarState::ACCELERATING_FORWARD)
			applyBoost();
	}
	else
	{
		overHeatTimer += frameTime;

		if (overHeatTimer >= BOOSTER_TIMEOUT)
		{
			boosterActive = true;
			overHeatTimer = 0.0f;
		}

		if (boostHeat > 0.0f)
		{
			// the booster cools down very slowly
			boostHeat -= frameTime / BOOST_RECHARGE_OVERHEAT;
		}
	}

	//////////////////////////////////////////////////////////////

	if (myEngine->KeyHeld(LEFT))
	{
		turnLeft();
		playerCarTurn = CarState::TURNING_LEFT;
	}

	if (myEngine->KeyHeld(RIGHT))
	{
		turnRight();
		playerCarTurn = CarState::TURNING_RIGHT;
	}

	// reset pitch and roll if no input is detected
	if (playerCarTurn != CarState::TURNING_LEFT && playerCarTurn != CarState::TURNING_RIGHT)
		resetRoll();

	if (playerCarAcel != CarState::ACCELERATING_BACKWARD && playerCarAcel != CarState::ACCELERATING_FORWARD)
		resetPitch();
	
	// update positions
	move();
	updateDummy(dummy);
	updateGridPoisition();

	// display debug text if F1 has been pressed
	if (showDebugInfo)
		displayInfo(debugFont);
}