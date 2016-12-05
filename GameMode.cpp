// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Globals.h"
#include "GameMode.h"
#include "MyCamera.h"
#include "RaceCourse.h"

using namespace tle;


// constructor
GameMode::GameMode(I3DEngine* myEngine) :
	myCamera(myEngine), raceCourse(myEngine), saving(false)
{
	raceCourse.createWorld(myEngine, 0.0f, SKYBOX_YPOS, 0.0f);
}

// virtual destructor so derived destructors are called
GameMode::~GameMode()
{
}