// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "Globals.h"
#include "Player_HoverCar.h"

using namespace tle;


class MyCamera
{
private:
	I3DEngine* myEngine;
	ICamera* myCamera;
	float defaultX;	// to the attached to item
	float defaultY;	// to the attached to item
	float defaultZ;	// to the attached to item
	float angleX;

	float cameraRotateY;	// stores the total rotation away from the cars facing direction
	float cameraRotateX;	// stores the total rotation away from the cars facing direction

	bool allowVerticalMovement = false;	// allows the camera to be rotated around the X axis

	// private default constructor
	MyCamera() {}

	// private copy constructor
	MyCamera(const MyCamera& copy) {}

public:
	// constructor - create the camera with optional x, y and z positions and an optional attach point
	MyCamera(I3DEngine* myEngine, ECameraType eCameraType = kManual, float xPos = 0.0f, float yPos = CAM_DEFAULT_HEIGHT, 
		float zPos = CAM_DEFAULT_Z_OFFSET, float angle = CAM_DEFAULT_ANGLE, IModel* parent = nullptr);

	// getters //

	ICamera* getModel() const { return myCamera; }	// returns a pointer to the camera to gain access to the standard functions
	float getCameraRotateY() const { return cameraRotateY; }
	float getCameraRotateX() const { return cameraRotateX; }

	// setters //

	void setParent(IModel* parent, float xOffset = 0.0f, float yOffset = CAM_DEFAULT_HEIGHT, 
		float zOffset = CAM_DEFAULT_Z_OFFSET, float angle = CAM_DEFAULT_ANGLE);

	// takes key presses and mouse movement and moves the camera around the car
	void moveCamera(float frameTimer);
	void moveCameraEditor(float frameTimer, string templateName, float templateScale);
	void resetCamPos(Player_HoverCar* playerHoverCar, IModel* dummy);
	void setCamRotationX(float rotation) { cameraRotateX = rotation; }
	void setCamRotationY(float rotation) { cameraRotateY = rotation; }
};