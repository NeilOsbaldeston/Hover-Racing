// Neil Osbaldeston

#include "Particle.h"
#include "Extras.h"
#include "Globals.h"
#include "Media.h"

// constructor
Particle::Particle(IMesh* mesh, float grav, int minVert, int maxVert,
	int minDir, int maxDir, int minVel, int maxVel, std::string skin) :
	myMesh(mesh), gravity(grav), minVertAngle(minVert), maxVertAngle(maxVert), 
	minDirAngle(minDir), maxDirAngle(maxDir), lifeTimer(0.0f), velocity(0.0f),
	minVelocity(minVel), maxVelocity(maxVel)
{
	myModel = myMesh->CreateModel();

	if (skin != "")
		myModel->SetSkin(skin);

	moveVector[0] = 0.0f;
	moveVector[1] = 0.0f;
	moveVector[2] = 0.0f;

	setHorizParticleVector();
}

// destructor
Particle::~Particle()
{
	myMesh->RemoveModel(myModel);
}

// set the initial vector upon creation or reset
void Particle::setHorizParticleVector(IModel* model)
{
	velocity = static_cast<float>(random(minVelocity, maxVelocity));

	if (velocity != 0.0f)
	{
		// get a random angle of travel (within a range)
		xyAngle = deg2Rads(static_cast<float>(random(minDirAngle, maxDirAngle)));
		zyAngle = deg2Rads(static_cast<float>(random(minVertAngle, maxVertAngle)));

		// work out the initial vectors from our random angles
		moveVector[0] = velocity * sin(zyAngle) * cos(xyAngle);
		moveVector[1] = velocity * sin(zyAngle) * sin(xyAngle);
		moveVector[2] = -velocity * cos(zyAngle);	// negative velocity to reverse the Z direction

		if (model != nullptr)
		{
			// get the cars direction vector and normalise it
			float matrix[4][4];
			model->GetMatrix(&matrix[0][0]);

			float zX = matrix[2][0];
			float zY = matrix[2][1];
			float zZ = matrix[2][2];

			float lengthZ = sqrtf(zX * zX + zY * zY + zZ * zZ);

			zX /= lengthZ;
			zY /= lengthZ;
			zZ /= lengthZ;

			// get the angle between the cars facing vector and the world Z axis
			float angle = safeAcosf(zZ);

			// cross product to get the sign (because we're testing against a world axis, its just zX)
			if (zX > 0.0f)
				angle = -angle;

			// rotate the particle vector around the Y axis by angle
			float tempX = moveVector[0] * cos(angle) - moveVector[2] * sin(angle);
			float tempZ = moveVector[0] * sin(angle) + moveVector[2] * cos(angle);

			moveVector[0] = tempX;
			moveVector[2] = tempZ;

			//// get the angle between the cars facing vector and the world Y axis
			//angle = safeAcosf(zY) + 0.174533f;	// 10 degrees

			//// rotate the particle vector around the X axis by angle
			//float tempY = moveVector[1] * cos(angle) + moveVector[2] * sin(angle);
			//tempZ = moveVector[1] * -sin(angle) + moveVector[2] * cos(angle);

			//moveVector[1] = tempY;
			//moveVector[2] = tempZ;
		}
	}
}

// set the initial vector upon creation or reset
void Particle::setVertParticleVector(IModel* model)
{
	velocity = static_cast<float>(random(minVelocity, maxVelocity));

	if (velocity != 0.0f)
	{
		// get a random angle of travel (within a range)
		xzAngle = deg2Rads(static_cast<float>(random(minDirAngle, maxDirAngle)));
		zyAngle = deg2Rads(static_cast<float>(random(minVertAngle, maxVertAngle)));

		// work out the initial vectors from our random angles
		moveVector[0] = velocity * sin(zyAngle) * cos(xyAngle);
		moveVector[1] = velocity * cos(zyAngle);
		moveVector[2] = velocity * sin(zyAngle) * sin(xyAngle);
	}
}

// move the particle according to its traits
void Particle::manipulateParticle(float frameTime)
{
	// apply gravity
	moveVector[1] -= gravity * frameTime;

	// move the particle
	myModel->SetX(myModel->GetX() + moveVector[0] * frameTime);
	myModel->SetY(myModel->GetY() + moveVector[1] * frameTime);
	myModel->SetZ(myModel->GetZ() + moveVector[2] * frameTime);

	// count towards the particles expire time
	increaseLifeTimer(frameTime);
}