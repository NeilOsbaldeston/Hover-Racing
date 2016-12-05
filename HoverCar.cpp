// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>	// for onscreen text
#include <iomanip>
#include "HoverCar.h"
#include "Player_HoverCar.h"
#include "AI_HoverCar.h"
#include "Globals.h"
#include "Extras.h"


using namespace tle;


// constructor
HoverCar::HoverCar(I3DEngine* myEngine, string mesh, RaceCourse* raceCourse, float xPos, float yPos, float zPos) :
	myEngine(myEngine), name(""), facingVelocity(0.0f), momentumVelocity(0.0f), speed(0.0f), fallSpeed(0.0f), 
	turnSpeed(0.0f), turnAngle(0.0f), rollAmount(0.0f), pitchAmount(0.0f), driftVector(), frameTime(0.0f), 
	collisionAngle(0.0f), bounceControl(0.0f), currentMaxFallSpeed(0.0f), maxFallSpeed(0.0f), fallSpeedTimer(0.0f), 
	hitPoints(STARTING_HITPOINTS), collisionRotation(false), rotationVelocity(0.0f), currentLap(1), currentStage(1), 
	checkPointDistance2(0.0f), finished(false)
{
	hoverCarMesh = myEngine->LoadMesh(mesh);
	hoverCar = hoverCarMesh->CreateModel(xPos, yPos, zPos);

	createEngineParticles();

	checkPointObjects = raceCourse->getCheckPointMaps();
	allBoxOjects = raceCourse->getBoxObjectMaps();
	allSphereOjects = raceCourse->getSphereObjectMaps();
	bombs = raceCourse->getBombs();

	updateGridPoisition();
}

// destructor
HoverCar::~HoverCar()
{
	for (int i = 0; i < ENGINE_PARTICLE_COUNT; ++i)
	{
		delete engineParticles[i];
		engineParticles[i] = nullptr;
	}

	hoverCarMesh->RemoveModel(hoverCar);
}

// displays all debug info (F1 during a race)
void HoverCar::displayInfo(IFont* debugFont)
{
	stringstream outText;
	outText << "Momentum speed: " << fixed << setprecision(2) << momentumVelocity;
	debugFont->Draw(outText.str(), 15, 35, kYellow);
	outText.str(""); // Clear myStream
	outText << "Turn speed: " << fixed << setprecision(2) << fabs(turnSpeed);
	debugFont->Draw(outText.str(), 15, 45, kYellow);
	outText.str(""); // Clear myStream
	outText << "Turn angle: " << fixed << setprecision(2) << turnAngle * 180.0f / PI;
	debugFont->Draw(outText.str(), 15, 55, kYellow);
	outText.str(""); // Clear myStream
	outText << "Roll amount: " << fixed << setprecision(2) << rollAmount;
	debugFont->Draw(outText.str(), 15, 65, kYellow);
	outText.str(""); // Clear myStream
	outText << "Pitch amount: " << fixed << setprecision(2) << pitchAmount;
	debugFont->Draw(outText.str(), 15, 75, kYellow);
	outText.str(""); // Clear myStream
	outText << "Drift vector X: " << fixed << setprecision(2) << driftVector[0];
	debugFont->Draw(outText.str(), 15, 85, kYellow);
	outText.str(""); // Clear myStream
	outText << "Drift vector Z: " << fixed << setprecision(2) << driftVector[2];
	debugFont->Draw(outText.str(), 15, 95, kYellow);
	outText.str(""); // Clear myStream
	outText << "Grid: " << gridZ << " , " << gridX;
	debugFont->Draw(outText.str(), 15, 105, kYellow);
	outText.str(""); // Clear myStream
	outText << "Collision angle: " << fixed << setprecision(2) << collisionAngle;
	debugFont->Draw(outText.str(), 15, 115, kYellow);
	outText.str(""); // Clear myStream
	outText << "Bounce control: " << fixed << setprecision(2) << bounceControl;
	debugFont->Draw(outText.str(), 15, 125, kYellow);
	outText.str(""); // Clear myStream
	outText << "Fall speed: " << fixed << setprecision(2) << fallSpeed * 100.0f;
	debugFont->Draw(outText.str(), 15, 135, kYellow);
	outText.str(""); // Clear myStream
	outText << "Max fall speed: " << fixed << setprecision(2) << maxFallSpeed * 100.0f;
	debugFont->Draw(outText.str(), 15, 145, kYellow);
	outText.str(""); // Clear myStream
	outText << "Current max fall speed: " << fixed << setprecision(2) << currentMaxFallSpeed * 100.0f;
	debugFont->Draw(outText.str(), 15, 155, kYellow);
	outText.str(""); // Clear myStream
}

// gets both player and AI cars to use for car to car collisions
void HoverCar::setCarLists(Player_HoverCar*& Player, vector<AI_HoverCar*>& AIs)
{
	hoverCarPlayer = Player;
	carsAI = AIs;
}


// movement //

void HoverCar::setFrameTime(float time)
{
	frameTime = time;
}

void HoverCar::move()
{
	collisionTurn();

	if (momentumVelocity > 0.0f)	// prevents dividing by 0
		findFacingSpeed();
	else
		facingVelocity = speed;

	applyDrag();
	drift();
	hover();

	// do collision checks and take care of collision resolution //

	if (checkCollisionGrids(allSphereOjects, &HoverCar::sphereToSphereCollisions) || 
		checkCollisionGrids(allBoxOjects, &HoverCar::sphereToBoxCollisions) ||
		checkCollisionGrids(checkPointObjects, &HoverCar::sphereToCheckPointCollisions) ||
		checkBoundaryCollision() || carToCarCollisions(hoverCarPlayer, carsAI))
	{
		// set to drift speed
		momentumVelocity = sqrtf(driftVector[0] * driftVector[0] + driftVector[2] * driftVector[2]);

		// for debug display use
		collisionAngle *= 180.0f / PI;

		float angle = collisionAngle;

		// determine rotation angle when hitting an object 
		// (imagine the front end of a car hitting a wall at an angle and the back ends momentum carrying around to also hit the wall)

		// corrects the angles //

		// hitting right side of a wall
		if (angle > 90.0f || angle < -90.0f)
		{
			if (angle > 90.0f)
				angle -= 90.0f;
			else if (angle < -90.0f)
				angle += 90.0f;

			if (angle > 45.0f && angle < 90.0f)
				angle = 90.0f - angle;
			else if (angle < -45.0f && angle > -90.0f)
				angle = -90.0f - angle;

			rotationVelocity += momentumVelocity * angle * ROTATION_MULTI;
		}
		else  // hitting left side of a wall
		{
			if (angle > 45.0f && angle < 90.0f)
				angle = 90.0f - angle;
			else if (angle < -45.0f && angle > -90.0f)
				angle = -90.0f - angle;

			rotationVelocity += momentumVelocity * (-angle) * ROTATION_MULTI;
		}

		// cap the collision rotation speed
		if (rotationVelocity > ROTATION_VEL_MAX)
			rotationVelocity = ROTATION_VEL_MAX;
		else if (rotationVelocity < -ROTATION_VEL_MAX)
			rotationVelocity = -ROTATION_VEL_MAX;
	}

	// car turning //

	// for debug text
	turnSpeed = (rollAmount / MAX_ROLL) * TURN_SPEED * frameTime;

	// turn the car left or right by a percentage of the car rolls amount.  If the car is going backwards the direction is revered
	if (facingVelocity > 0.001f)
	{
		hoverCar->RotateY(turnSpeed);
	}
	else if (facingVelocity < -0.001f)
	{
		hoverCar->RotateY(-turnSpeed);
	}
}

void HoverCar::updateDummy(IModel* dummy)	// could move to player....
{
	dummy->SetX(hoverCar->GetX());
	dummy->SetY(hoverCar->GetY());
	dummy->SetZ(hoverCar->GetZ());
	dummy->ResetOrientation();

	// get current facing vector
	float matrix[4][4];

	hoverCar->GetMatrix(&matrix[0][0]);

	// get the length of the cars local Z vector in the x and z plane only
	float vectorLength = sqrtf((matrix[2][0] * matrix[2][0]) + (matrix[2][2] * matrix[2][2]));

	// normalise the z component of the cars local Z vector
	float unitZz = matrix[2][2] / vectorLength;

	// use the dot product to get the angle between the carZ vector and the default Z vector
	turnAngle = acos(unitZz);

	// the angle is from 0 - 180 and always positive (shortest path), so i find out if the car is facing left or right of north facing (z(0,0,1))
	if (matrix[2][0] < 0.0f)
	{
		turnAngle = -turnAngle;
	}

	// convert the angle from radians to degrees and rotate the dummy to match the car
	dummy->RotateY(turnAngle * 180.0f / PI);
}


// car turning //

void HoverCar::turnLeft(float alpha)
{
	// checks to see if the turn is within limits, then deducted from the roll amount for future reference and the car is rolled left
	if (rollAmount - alpha * frameTime >= -MAX_ROLL)
	{
		float temp = pitchAmount;
		hoverCar->RotateLocalX(-temp);	// reset the pitch to prevent an accumulation of offset movement

		rollAmount -= alpha * frameTime;
		hoverCar->RotateLocalZ(alpha * frameTime);

		hoverCar->RotateLocalX(temp);	// re-pitch the car
	}
}

void HoverCar::turnRight(float alpha)
{
	// checks to see if the turn is within limits, then added to the roll amount for future reference and the car is rolled right
	if (rollAmount + alpha * frameTime <= MAX_ROLL)
	{
		float temp = pitchAmount;
		hoverCar->RotateLocalX(-temp);	// reset the pitch to prevent an accumulation of offset movement

		rollAmount += alpha * frameTime;
		hoverCar->RotateLocalZ(-alpha * frameTime);

		hoverCar->RotateLocalX(temp);	// re-pitch the car
	}
}

void HoverCar::resetRoll()
{
	float temp = pitchAmount;
	hoverCar->RotateLocalX(-temp);	// reset the pitch to prevent an accumulation of offset movement

	// if the car is not up-right, it is rolled back towards up-right but only if the car is not being turned
	if (rollAmount < -RESET_ROLL_SPEED * frameTime)
	{
		rollAmount += RESET_ROLL_SPEED * frameTime;
		hoverCar->RotateLocalZ(-RESET_ROLL_SPEED * frameTime);
	}
	else if (rollAmount > RESET_ROLL_SPEED * frameTime)
	{
		rollAmount -= RESET_ROLL_SPEED * frameTime;
		hoverCar->RotateLocalZ(RESET_ROLL_SPEED * frameTime);
	}

	hoverCar->RotateLocalX(temp);	// re-pitch the car
}

// the result of a collision, the effect of momentum carrying the weight 
// of the vehicle into the wall in which ever direction is available
void HoverCar::collisionTurn()
{
	if (rotationVelocity > ROTATION_VEL_MIN || rotationVelocity < -ROTATION_VEL_MIN)
		hoverCar->RotateY(rotationVelocity * frameTime);
	
	if (rotationVelocity > 0.0f)
		rotationVelocity -= frameTime * ROTATION_VEL_DECAY;
	else if (rotationVelocity < 0.0f)
		rotationVelocity += frameTime * ROTATION_VEL_DECAY;
}


// car momentum //

void HoverCar::accelerateForward(float maxSpeed, float acceleration, float alpha)
{
	// when the car accelerates forward, the nose pitches up until it reaches its limit and stores the pitch amount 
	// with each iteration.  The acceleration factor is multiplied by a percentage of the pitch amount and added to facingSpeed
	if (pitchAmount - alpha * frameTime >= -MAX_BACKWARD_PITCH)
	{
		pitchAmount -= alpha * frameTime;
		hoverCar->RotateLocalX(-alpha * frameTime);
	}

	if (facingVelocity < maxSpeed)
	{
		if (pitchAmount >= 0.0f)
			speed += acceleration * (pitchAmount / MAX_BACKWARD_PITCH) * frameTime;
		else
			speed += acceleration * (-pitchAmount / MAX_BACKWARD_PITCH) * frameTime;
	}

	getDriftVector();
}

void HoverCar::accelerateBackward(float maxSpeed, float acceleration, float alpha)
{
	// when the car accelerates backwards, the nose pitches down until it reaches its limit and stores the pitch amount with each 
	// iteration.  The acceleration factor is multiplied by a percentage of the pitch amount and deducted from the current speed
	if (pitchAmount + alpha * frameTime <= MAX_FOWARD_PITCH)
	{
		pitchAmount += alpha * frameTime;
		hoverCar->RotateLocalX(alpha * frameTime);
	}

	if (facingVelocity > maxSpeed)
	{
		if (pitchAmount >= 0.0f)
			speed -= acceleration * (pitchAmount / MAX_FOWARD_PITCH) * frameTime;
		else
			speed -= acceleration * (-pitchAmount / MAX_FOWARD_PITCH) * frameTime;
	}

	getDriftVector();
}

// resets pitch back to 0
void HoverCar::resetPitch()
{
	if (pitchAmount < -RESET_PITCH_SPEED * frameTime)
	{
		pitchAmount += RESET_PITCH_SPEED * frameTime;
		hoverCar->RotateLocalX(RESET_PITCH_SPEED * frameTime);
	}
	else if (pitchAmount > RESET_PITCH_SPEED * frameTime)
	{
		pitchAmount -= RESET_PITCH_SPEED * frameTime;
		hoverCar->RotateLocalX(-RESET_PITCH_SPEED * frameTime);
	}
}

// show the speed your travelling in the direction your facing
void HoverCar::findFacingSpeed()
{
	// get current facing vector
	float matrix[4][4];
	hoverCar->GetMatrix(&matrix[0][0]);

	float distance = sqrtf(matrix[2][0] * matrix[2][0] + matrix[2][2] * matrix[2][2]);

	// normalise vector
	matrix[2][0] /= distance;
	matrix[2][2] /= distance;

	// normalise vector
	float driftX = driftVector[0] / momentumVelocity;
	float driftZ = driftVector[2] / momentumVelocity;

	float dotProduct = matrix[2][0] * driftX + matrix[2][2] * driftZ;

	float alpha = safeAcosf(dotProduct);	// absolute value
	float degrees = alpha * 180.0f / PI;

	// take the angle between the facing vector and the momentum vector and work out the facing speed
	facingVelocity = (((90.0f - degrees) / 100.0f) * momentumVelocity)/* + speed*/;
}

// reduces thrust effect
void HoverCar::applyDrag()
{
	//speed *= 1.0f - (FORWARD_DRAG * frameTime);	// 1.3 FORWARD_DRAG

	speed *= pow(FORWARD_DRAG, frameTime);	// 0.3 FORWARD_DRAG
}

void HoverCar::getDriftVector()
{
	// get current facing vector
	float matrix[4][4];

	hoverCar->GetMatrix(&matrix[0][0]);

	float facingVectorLength = sqrtf((matrix[2][0] * matrix[2][0]) + (matrix[2][2] * matrix[2][2]));

	// normalise the x and z components of the cars local Z vector, multiply by facing speed and add to the drift vector
	driftVector[0] += (matrix[2][0] / facingVectorLength) * speed * frameTime;
	driftVector[2] += (matrix[2][2] / facingVectorLength) * speed * frameTime;

	//  get the new length of the vector
	momentumVelocity = sqrtf((driftVector[0] * driftVector[0]) + (driftVector[2] * driftVector[2]));

	// safe guard again fast speeds
	if (momentumVelocity > FORWARD_MAX_SPEED)
	{	
		driftVector[0] = (driftVector[0] / momentumVelocity) * FORWARD_MAX_SPEED;
		driftVector[2] = (driftVector[2] / momentumVelocity) * FORWARD_MAX_SPEED;
		momentumVelocity = FORWARD_MAX_SPEED;
	}
}

// apply the drift drag and move the car
void HoverCar::drift()
{
	// apply drag to the current drift
	if (momentumVelocity > 0.0f)
	{
		driftVector[0] *= pow(DRIFT_DRAG, frameTime);	// 0.4 DRIFT_DRAG
		driftVector[2] *= pow(DRIFT_DRAG, frameTime);
		
		// for debug text
		momentumVelocity = sqrtf((driftVector[0] * driftVector[0]) + (driftVector[2] * driftVector[2]));
	}

	// move the car
	hoverCar->MoveX(driftVector[0] * MOVE_MULTIPLIER * frameTime);
	hoverCar->MoveZ(driftVector[2] * MOVE_MULTIPLIER * frameTime);
}

// apply gravity and anti gravity to the car
void HoverCar::hover()
{
	float gravity = GRAVITY;

	// get height from ground
	float height = hoverCar->GetY();

	// negate fallSpeed with hover if in range
	if (height < HOVER_STRENGTH_MIN)
	{
		float percentStrength = (HOVER_STRENGTH_MIN - height) / (HOVER_STRENGTH_MIN - HOVER_STRENGTH_FULL);	// i.e. 0.23

		// if on the rebound, reduce porpoising
		if ((percentStrength > PORPOISE_CONTROL && fallSpeed < 0.0f) || (percentStrength > PORPOISE_CONTROL && fallSpeed > 0.0f))
		{
			fallSpeed += -fallSpeed * percentStrength * STRENGTH_MULTI * frameTime;
		}

		// when the car settles down, reduce bounce speed
		if (percentStrength > GRAVITY_CHANGE_MIN && percentStrength < GRAVITY_CHANGE_MAX)
			gravity = LOW_GRAVITY;

		// apply anti gravity
		fallSpeed -= gravity * percentStrength * frameTime;

		// debug text
		{
			fallSpeedTimer += frameTime;
			if (fallSpeedTimer >= 2.0f)
			{
				fallSpeedTimer = 0.0f;
				currentMaxFallSpeed = 0.0f;
			}
			if (fallSpeed > currentMaxFallSpeed)
				currentMaxFallSpeed = fallSpeed;

			if (fallSpeed > maxFallSpeed)
				maxFallSpeed = fallSpeed;
		}
	}

	// apply gravity
	fallSpeed += gravity * frameTime;

	// limit fall speed (terminal velocity)
	if (fallSpeed >= TERMINAL_VELOCITY)
		fallSpeed = TERMINAL_VELOCITY;

	// move the car according to gravity and anti gravity
	hoverCar->MoveY(-fallSpeed);
}


// collisions //

void HoverCar::updateGridPoisition()
{
	gridX = static_cast<int>((hoverCar->GetX() + (MAP_GRID_SIZE * SECTOR_SIZE / 2)) / SECTOR_SIZE);	// columns
	gridZ = static_cast<int>((hoverCar->GetZ() + (MAP_GRID_SIZE * SECTOR_SIZE / 2)) / SECTOR_SIZE);	// rows
	grid = gridX + (gridZ * MAP_GRID_SIZE);
}

// detects collisions between the car and all objects with a box collision volume in detection range
bool HoverCar::sphereToBoxCollisions(Box*& object, float objX, float objZ)
{
	float carX = hoverCar->GetX();
	float carZ = hoverCar->GetZ();

	float objectLeft = 0.0f;
	float objectRight = 0.0f;
	float objectFront = 0.0f;
	float objectBack = 0.0f;

	float objMatrix[4][4];
	object->model->GetMatrix(&objMatrix[0][0]);

	// if the object is facing default direction
	if (objMatrix[0][0] >= 0.9f * object->scalar)
	{
		objectLeft = (objX - (object->width * object->scalar / 2.0f)) - HOVERCAR_RADIUS;	
		objectRight = (objX + (object->width * object->scalar / 2.0f)) + HOVERCAR_RADIUS;
		objectFront = (objZ - (object->length * object->scalar / 2.0f)) - HOVERCAR_RADIUS;
		objectBack = (objZ + (object->length * object->scalar / 2.0f)) + HOVERCAR_RADIUS;
	}
	else // if the object has been rotated around the Y axis
	{
		objectLeft = (objX - (object->length * object->scalar / 2.0f)) - HOVERCAR_RADIUS;
		objectRight = (objX + (object->length * object->scalar / 2.0f)) + HOVERCAR_RADIUS;
		objectFront = (objZ - (object->width * object->scalar / 2.0f)) - HOVERCAR_RADIUS;
		objectBack = (objZ + (object->width * object->scalar / 2.0f)) + HOVERCAR_RADIUS;
	}

	if (carX > objectLeft && carX < objectRight && carZ > objectFront && carZ < objectBack)
	{
		// collision resolution //

		// reverse movement by a tenth until no longer a collision
		while (carX > objectLeft && carX < objectRight && carZ > objectFront && carZ < objectBack)
		{
			// reverse the last drift move
			hoverCar->MoveX((-driftVector[0] * MOVE_MULTIPLIER * frameTime) / COL_RES_DIVISION);
			hoverCar->MoveZ((-driftVector[2] * MOVE_MULTIPLIER * frameTime) / COL_RES_DIVISION);

			carX = hoverCar->GetX();
			carZ = hoverCar->GetZ();
		}

		// work out new drift direction
		boxCollisionResolution(carX, carZ, objectLeft, objectRight, objectFront, objectBack);

		return true;
	}
	return false;
}

// detects collisions between the car and all the check points in detection range
// pillar detection is sphere to sphere
bool HoverCar::sphereToCheckPointCollisions(Sphere*& object, float objX, float objZ)
{
	float cpMatrix[4][4];	// check point matrix
	object->model->GetMatrix(&cpMatrix[0][0]);

	// X vector along x and z plane
	float objXx = cpMatrix[0][0];
	float objXz = cpMatrix[0][2];

	// length of the Z (facing) vector along its x and z axis only
	float vecLength = sqrt(objXx * objXx + objXz * objXz);

	// normalise the vector
	objXx /= vecLength;
	objXz /= vecLength;

	float distance = CHECKPOINT_WIDTH * object->scalar / 2.0f;

	// take a point to the left and to the right of the object position along its X axis
	float leftX = cpMatrix[3][0] - objXx * distance;
	float leftZ = cpMatrix[3][2] - objXz * distance;

	float rightX = cpMatrix[3][0] + objXx * distance;
	float rightZ = cpMatrix[3][2] + objXz * distance;

	// check collision against each pillar
	if (sphereToSphereCollisions(object, leftX, leftZ))
		return true;
	if (sphereToSphereCollisions(object, rightX, rightZ))
		return true;

	return false;
}

// stops the car from travelling beyond the skybox 
bool HoverCar::checkBoundaryCollision()
{
	float carX = hoverCar->GetX();
	float carZ = hoverCar->GetZ();

	float objectLeft = -SKYBOX_WIDTH / 2.0f + SKYBOX_BOUNDARY - HOVERCAR_RADIUS;
	float objectRight = SKYBOX_WIDTH / 2.0f - SKYBOX_BOUNDARY + HOVERCAR_RADIUS;
	float objectFront = -SKYBOX_WIDTH / 2.0f + SKYBOX_BOUNDARY - HOVERCAR_RADIUS;
	float objectBack = SKYBOX_WIDTH / 2.0f - SKYBOX_BOUNDARY + HOVERCAR_RADIUS;

	if (carX < objectLeft || carX > objectRight || carZ < objectFront || carZ > objectBack)
	{
		// collision resolution //

		boxCollisionResolution(carX, carZ, objectLeft, objectRight, objectFront, objectBack);

		return true;
	}
	return false;
}

// sphere to sphere
bool HoverCar::carToCarCollisions(Player_HoverCar* hoverCarPlayer, vector<AI_HoverCar*> carsAI)
{
	// get all cars other than 'this' car
	vector<HoverCar*> cars;	// generic car list

	if (this->getModel() == hoverCarPlayer->getModel())
	{
		for (int car = 0; car < static_cast<int>(carsAI.size()); ++car)
		{
			cars.push_back(dynamic_cast<HoverCar*>(carsAI[car]));
		}
	}
	else
	{
		cars.push_back(dynamic_cast<HoverCar*>(hoverCarPlayer));
		for (int car = 0; car < static_cast<int>(carsAI.size()); ++car)
		{
			if (this->getModel() != carsAI[car]->getModel())
				cars.push_back(dynamic_cast<HoverCar*>(carsAI[car]));
		}
	}

	// iterate through each car in the list
	for (auto car = cars.begin(); car != cars.end(); ++car)
	{
		float distance2 = 0.0f;	// distance squared

		float vectorX = (*car)->getModel()->GetX() - hoverCar->GetX();
		float vectorZ = (*car)->getModel()->GetZ() - hoverCar->GetZ();

		// using distance squared to save on computing the square root as done many times
		distance2 = vectorX * vectorX + vectorZ * vectorZ;

		if (distance2 <= HOVERCAR_RADIUS2 + HOVERCAR_RADIUS2)
		{
			// collision resolution //

			// set local variables
			float directionX = driftVector[0];
			float directionZ = driftVector[2];

			// reverse movement by a tenth until no longer a collision
			while (distance2 <= HOVERCAR_RADIUS2 + HOVERCAR_RADIUS2)
			{
				// reverse the last drift move
				hoverCar->MoveX((-directionX * MOVE_MULTIPLIER * frameTime) / COL_RES_DIVISION);
				hoverCar->MoveZ((-directionZ * MOVE_MULTIPLIER * frameTime) / COL_RES_DIVISION);

				vectorX = (*car)->getModel()->GetX() - hoverCar->GetX();
				vectorZ = (*car)->getModel()->GetZ() - hoverCar->GetZ();

				// using distance squared to save on computing the square root as done many times
				distance2 = vectorX * vectorX + vectorZ * vectorZ;
			}

			// normalise
			directionX /= momentumVelocity;
			directionZ /= momentumVelocity;

			float distance = sqrt(distance2);
			vectorX /= distance;
			vectorZ /= distance;

			float dotProduct = directionX * vectorX + directionZ * vectorZ;

			// returns an absolute angle
			collisionAngle = acos(dotProduct);

			// orthogonal vector to the x and z vectors (returns a positive if the angle 
			// is collision angle is positive else it will return a negative)
			float crossProduct = directionZ * vectorX - vectorZ * directionX;

			// if the angle should be a negative
			if (crossProduct < 0.0f)
			{
				collisionAngle = -collisionAngle;
			}

			float sinAlpha = sin(collisionAngle);
			float cosAlpha = cos(collisionAngle);

			// store orininal vectors of opposing car
			float dVX = (*car)->driftVector[0];
			float dVZ = (*car)->driftVector[2];

			// Rotate the reversed collision vector and add it to the other cars vector to give it a new drift vector
			(*car)->driftVector[0] += (vectorX * cosAlpha + vectorZ * sinAlpha) / BOUNCE_CAR_DAMPENING;
			(*car)->driftVector[2] += (vectorX * -sinAlpha + vectorZ * cosAlpha) / BOUNCE_CAR_DAMPENING;
			(*car)->momentumVelocity += (sqrtf(driftVector[0] * driftVector[0] + 
				driftVector[2] * driftVector[2]) - (*car)->momentumVelocity) / BOUNCE_CAR_DAMPENING;

			// cap the speed
			if ((*car)->momentumVelocity > FORWARD_MAX_SPEED)
			{
				(*car)->momentumVelocity = FORWARD_MAX_SPEED;
			}

			// set drift vector to the reversed vector but at a reduced rate (inertia has been transfered to the other car)
			driftVector[0] = dVX + ((-vectorX * cosAlpha - vectorZ * sinAlpha) / (BOUNCE_CAR_DAMPENING * BOUNCE_CAR_DAMPENING));
			driftVector[2] = dVZ + ((-vectorX * -sinAlpha - vectorZ * cosAlpha) / (BOUNCE_CAR_DAMPENING * BOUNCE_CAR_DAMPENING));

			return true;
		}
	}
	return false;
}

// sphere to box collision resolution
void HoverCar::boxCollisionResolution(float carX, float carZ, float objectLeft, 
	float objectRight, float objectFront, float objectBack)
{
	float directionX = driftVector[0];
	float directionZ = driftVector[2];

	// normalise
	directionX /= momentumVelocity;
	directionZ /= momentumVelocity;

	// returns an absolute angle
	collisionAngle = acos(directionX);	// direction x is also the dot product in this case

	// orthogonal vector to the x and z vectors (returns a positive if the angle is collision angle is positive else it will return a negative)
	// checking in relation to the X axis
	float crossProduct = directionZ;

	// if the angle should be a negative
	if (crossProduct < 0.0f)
	{
		collisionAngle = -collisionAngle;
	}

	float sinAlpha = sin(collisionAngle);
	float cosAlpha = cos(collisionAngle);

	// fast bounce when speed is fast and vice versa. (division is to reduce the amount of bounce)
	bounceControl = sqrt(driftVector[0] * driftVector[0] + driftVector[2] * driftVector[2]) / BOUNCE_DAMPENING + 0.02f;

	// if the car collides off the left or right side of the box
	if (carX - objectLeft < carZ - objectFront && carX - objectLeft < objectBack - carZ ||
		objectRight - carX < carZ - objectFront && objectRight - carX < objectBack - carZ)
	{
		// Rotate the reversed collision vector to give a new drift vector
		driftVector[0] = -cosAlpha * bounceControl;
		driftVector[2] = sinAlpha * bounceControl;
	}
	else  // if the player collides off the near or far sides of the box
	{
		// Rotate the reversed collision vector to give a new drift vector
		driftVector[0] = cosAlpha * bounceControl;
		driftVector[2] = -sinAlpha * bounceControl;

		if (collisionAngle > 0.0f)
			collisionAngle -= PI / 2.0f;
		else
			collisionAngle += PI / 2.0f;
	}

	//// make the car slide along a wall //

	////float matrix[4][4];
	////hoverCarPlayer->getModel()->GetMatrix(&matrix[0][0]);

	//// hitting right side of a wall
	//if (collisionAngle > PI / 2.0f || collisionAngle < -PI / 2.0f)
	//{
	//	if (collisionAngle > 0.0f)
	//	{
	//		if (lastCol == CollisionDirection::RIGHT_SIDE_MINUS)
	//			hoverCar->MoveLocalZ(-10.0f * frameTime);

	//		hoverCar->MoveLocalX(10.0f * frameTime);
	//		lastCol = CollisionDirection::RIGHT_SIDE_PLUS;
	//	}
	//	else
	//	{
	//		if (lastCol != CollisionDirection::RIGHT_SIDE_MINUS)
	//			hoverCar->MoveLocalZ(-10.0f * frameTime);

	//		hoverCar->MoveLocalX(-10.0f * frameTime);
	//		lastCol = CollisionDirection::RIGHT_SIDE_MINUS;
	//	}
	//}
	//else  // hitting left side
	//{
	//	if (collisionAngle > 0.0f)
	//	{
	//		if (lastCol != CollisionDirection::LEFT_SIDE_MINUS)
	//			hoverCar->MoveLocalZ(-10.0f * frameTime);

	//		hoverCar->MoveLocalX(-10.0f * frameTime);
	//		lastCol = CollisionDirection::LEFT_SIDE_MINUS;
	//	}
	//	else
	//	{
	//		if (lastCol != CollisionDirection::LEFT_SIDE_PLUS)
	//			hoverCar->MoveLocalZ(-10.0f * frameTime);

	//		hoverCar->MoveLocalX(10.0f * frameTime);
	//		lastCol = CollisionDirection::LEFT_SIDE_PLUS;
	//	}
	//}

	// reduce hit point by an amount appropriate to the collision impact
	float damage = DAMAGE_MULTIPLIER * fabs(momentumVelocity * (1.0f + fabs(collisionAngle)));

	if (damage >= 1.0f)
		hitPoints -= damage;
}

// detects collisions between the car and all objects with a spherical collision volume in detection range
bool HoverCar::sphereToSphereCollisions(Sphere*& object, float objX, float objZ)
{
	float carX = hoverCar->GetX();
	float carZ = hoverCar->GetZ();

	float distance2 = 0.0f;	// distance squared

	float vectorX = objX - carX;
	float vectorZ = objZ - carZ;

	// using distance squared to save on computing the square root as done many times
	distance2 = vectorX * vectorX + vectorZ * vectorZ;

	bool collision = false;
	if (distance2 <= object->radius2 * (object->scalar * object->scalar) + HOVERCAR_RADIUS2)	// scalar x scalar gives the equivalent radius squared
	{
		collision = true;
	}
	// if the collision is with a bomb, explode the bomb
	float bounceAjustment = 0.01f;
	for (Bomb* bomb : bombs)
	{
		if (object->model == bomb->getModel())
		{
			if (bomb->getActivated())
			{
				if (distance2 < BOMB_BLAST_RADIUS2)
				{
					collision = true;
				}
			}
		}
	}

	if (collision)
	{
		// collision resolution //
		bool explosion = false;
		for (Bomb* bomb : bombs)
		{
			if (object->model == bomb->getModel())
			{
				if (!bomb->getActivated())
				{
					object->model->MoveY(-1000.0f);
					bomb->setActivated(true);
				}
				explosion = true;
				bounceAjustment = (BOMB_BLAST_RADIUS2 / distance2) * BOMB_BLAST_STRENGTH * frameTime;
				hitPoints -= (BOMB_BLAST_RADIUS2 / distance2) * BOMB_BLAST_DAMAGE * frameTime;
			}
		}

		float directionX = driftVector[0];
		float directionZ = driftVector[2];

		float colRad2 = object->radius2 * (object->scalar * object->scalar) + HOVERCAR_RADIUS2;

		// reverse movement by a tenth until no longer a collision
		while (!explosion && distance2 <= colRad2)
		{
			// reverse the last drift move
			hoverCar->MoveX((-driftVector[0] * MOVE_MULTIPLIER * frameTime) / COL_RES_DIVISION);
			hoverCar->MoveZ((-driftVector[2] * MOVE_MULTIPLIER * frameTime) / COL_RES_DIVISION);

			vectorX = objX - hoverCar->GetX();
			vectorZ = objZ - hoverCar->GetZ();

			// using distance squared to save on computing the square root as done many times
			distance2 = vectorX * vectorX + vectorZ * vectorZ;
		}

		// normalise
		directionX /= momentumVelocity;
		directionZ /= momentumVelocity;

		float distance = sqrt(distance2);
		vectorX /= distance;
		vectorZ /= distance;

		float dotProduct = directionX * vectorX + directionZ * vectorZ;

		// returns an absolute angle
		collisionAngle = acosf(dotProduct);

		// orthogonal vector to the x and z vectors (returns a positive if the angle is collision angle is positive else it will return a negative)
		float crossProduct = directionZ * vectorX - vectorZ * directionX;

		float sinAlpha = 0.0f;
		float cosAlpha = 0.0f;

		// if the angle should be a negative
		if (crossProduct < 0.0f)
		{
			collisionAngle = -collisionAngle;
		}

		sinAlpha = sin(collisionAngle);
		cosAlpha = cos(collisionAngle);

		// fast bounce when speed is fast and vice versa. (division is to reduce the amount of bounce)
		bounceControl = sqrt(driftVector[0] * driftVector[0] + driftVector[2] * driftVector[2]) / BOUNCE_DAMPENING + bounceAjustment;

		// Rotate the reversed collision vector to give a new drift vector
		driftVector[0] = (-vectorX * cosAlpha - vectorZ * sinAlpha) * bounceControl;
		driftVector[2] = (-vectorX * -sinAlpha - vectorZ * cosAlpha) * bounceControl;

		if (!explosion)
		{
			// reduce hit point by an amount appropriate to the collision impact
			float damage = DAMAGE_MULTIPLIER * fabs(momentumVelocity * (DAMAGE_MULTIPLIER - fabs(collisionAngle)));

			if (damage >= 1.0f)
				hitPoints -= damage;
		}

		return true;
	}
	return false;
}


// particles //

void HoverCar::createEngineParticles()
{
	particleMesh = myEngine->LoadMesh(PARTICLE);

	for (int i = 0; i < ENGINE_PARTICLE_COUNT; ++i)
	{
		engineParticles.push_back(new Particle{
			particleMesh, 
			ENGINE_PARTICLE_GRAVITY, 
			ENGINE_PARTICLE_MIN_SPRAY_ANGLE, 
			ENGINE_PARTICLE_MAX_SPRAY_ANGLE, 
			ENGINE_PARTICLE_MIN_CONE_ANGLE, 
			ENGINE_PARTICLE_MAX_CONE_ANGLE, 
			ENGINE_PARTICLE_MIN_VEL, 
			ENGINE_PARTICLE_MAX_VEL, 
			FIRE_ORANGE});
	}
}

// set the initial positions at the cars exhaust
void HoverCar::setEngineParticlePositions(Particle* particle, IModel* model)
{
	float matrix[4][4];
	hoverCar->GetMatrix(&matrix[0][0]);

	particle->getModel()->SetMatrix(&matrix[0][0]);

	// set a random location within the thruster port
	float radius = random(0, ENGINE_PARTICLE_STARTING_MAX_RADIUS);
	float moveX = random(static_cast<int>(-radius), static_cast<int>(radius)) / 100.0f + ENGINE_PARTICLE_EXHAUST_X_OFFSET;
	float moveY = sqrtf((radius / 100.0f) * (radius / 100.0f) - moveX * moveX);
	float moveZ = random(-ENGINE_PARTICLE_Z_OFFSET, ENGINE_PARTICLE_Z_OFFSET) / 100.0f;

	// find out if we want a positive Y or negative Y
	int sign = static_cast<int>(random(0, 1) + 0.5f);

	// get the particle to the exhaust
	particle->getModel()->MoveLocalY(ENGINE_PARTICLE_EXHAUST_Y);
	particle->getModel()->MoveLocalZ(ENGINE_PARTICLE_EXHAUST_Z);

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

	particle->getModel()->Scale(ENGINE_PARTICLE_SCALE);
}

// after the particle has been travelling for a period of time, reset it
void HoverCar::expireEngineParticles(Particle* particle, IModel* model)
{
	if (particle->getLifeTimer() > ENGINE_PARTICLE_LIFETIME)
	{		
		setEngineParticlePositions(particle, model);

		// set lifeTimer back to zero
		particle->resetLifeTimer();
	}
}