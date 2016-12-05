// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "MyCamera.h"

using namespace tle;


// constructor - create the camera with optional x, y and z positions and an optional attach point
MyCamera::MyCamera(I3DEngine* myEngine, ECameraType eCameraType, float xPos, float yPos, float zPos, float angle, IModel* parent) :
	myEngine(myEngine), defaultX(xPos), defaultY(yPos), defaultZ(zPos), angleX(angle), cameraRotateY(0.0f), cameraRotateX(0.0f)
{
	myCamera = myEngine->CreateCamera(eCameraType, xPos, yPos, zPos);

	if (parent != nullptr)
	{
		myCamera->AttachToParent(parent);
		myCamera->SetLocalPosition(xPos, yPos, zPos);
		myCamera->RotateX(angle);
	}
}

void MyCamera::setParent(IModel* parent, float xOffset, float yOffset, float zOffset, float angle)
{
	myCamera->AttachToParent(parent);
	myCamera->SetLocalPosition(xOffset, yOffset, zOffset);
	myCamera->RotateLocalX(angle);
}

// takes key presses and mouse movement and moves the camera around the car
void MyCamera::moveCamera(float frameTimer)
{
	// get the mouse movement since the last call
	float mouseMoveX = static_cast<float>(myEngine->GetMouseMovementX());
	float mouseMoveY = static_cast<float>(myEngine->GetMouseMovementY());

	// find how much to move this frame
	float xMovement = mouseMoveX * CAM_ROTATE_SPEED;
	float yMovement = mouseMoveY * CAM_ROTATE_SPEED;

	// store the total rotation
	cameraRotateX += xMovement;
	cameraRotateY += yMovement;

	// rotate the camera
	myCamera->RotateY(xMovement);
	myCamera->RotateLocalX(yMovement);

	// move the camera along its local axis	 
	if (myEngine->KeyHeld(CAM_FORWARD))
		myCamera->MoveLocalZ(CAM_MOVE_SPEED * frameTimer);

	if (myEngine->KeyHeld(CAM_BACKWARD))
		myCamera->MoveLocalZ(-CAM_MOVE_SPEED * frameTimer);

	if (myEngine->KeyHeld(CAM_LEFT))
		myCamera->MoveLocalX(-CAM_MOVE_SPEED * frameTimer);

	if (myEngine->KeyHeld(CAM_RIGHT))
		myCamera->MoveLocalX(CAM_MOVE_SPEED * frameTimer);
}

// keys for camera movement along the axis. Camera rotation is handled only when the cursor is next to the edge of the screen
void MyCamera::moveCameraEditor(float frameTimer, string templateName, float templateScale)
{
	int correctionX = 0;
	if (FULLSCREEN)
		correctionX = BORDER_CORRECTION;

	int xPos = myEngine->GetMouseX() + correctionX;
	if (xPos < CAM_EDITOR_SIDE_WIDTH)
	{
		// increase the amount moved, the closer to the edge of the screen the cursor gets, up to a max CAM_ROTATE_SPEED_EDITOR
		float amount = ((CAM_EDITOR_SIDE_WIDTH - static_cast<float>(xPos)) * -CAM_ROTATE_SPEED_EDITOR / CAM_EDITOR_SIDE_WIDTH) * frameTimer;
		cameraRotateY = amount;
		myCamera->RotateY(amount);
	}
	xPos = myEngine->GetMouseX() + correctionX;
	if (xPos > myEngine->GetWidth() - static_cast<int>(CAM_EDITOR_SIDE_WIDTH))
	{
		float amount = ((CAM_EDITOR_SIDE_WIDTH - static_cast<float>((myEngine->GetWidth() - xPos))) * CAM_ROTATE_SPEED_EDITOR / CAM_EDITOR_SIDE_WIDTH) * frameTimer;
		cameraRotateY = amount;
		myCamera->RotateY(amount);
	}

	// rotation of the camera around the X axis
	if (myEngine->KeyHit(Key_F4))
	{
		if (!allowVerticalMovement)
			allowVerticalMovement = true;
		else
			allowVerticalMovement = false;
	}

	if (allowVerticalMovement)
	{
		if (myEngine->GetMouseY() < 10.0f)
		{
			cameraRotateX = CAM_ROTATE_SPEED_EDITOR * frameTimer;
			myCamera->RotateLocalX(-CAM_ROTATE_SPEED_EDITOR * frameTimer);
		}
		if (myEngine->GetMouseY() > myEngine->GetHeight() - 50.0f)
		{
			cameraRotateX = -CAM_ROTATE_SPEED_EDITOR * frameTimer;
			myCamera->RotateLocalX(CAM_ROTATE_SPEED_EDITOR * frameTimer);
		}
	}

	// while hold the WALL_OVERIDE button when holding a wall template, 
	// tapping forward will jump the camera forward by the exact length of the wall piece
	if (templateName == "Wall.x" && myEngine->KeyHeld(WALL_OVERIDE))
	{
		if (myEngine->KeyHit(FORWARD))
			myCamera->MoveLocalZ(WALL_LENGTH * templateScale);

		if (myEngine->KeyHit(BACKWARD))
			myCamera->MoveLocalZ(-WALL_LENGTH * templateScale);
	}
	else
	{
		if (myEngine->KeyHeld(FORWARD))
			myCamera->MoveLocalZ(CAM_MOVE_SPEED * frameTimer);

		if (myEngine->KeyHeld(BACKWARD))
			myCamera->MoveLocalZ(-CAM_MOVE_SPEED * frameTimer);
	}

	if (myEngine->KeyHeld(LEFT))
		myCamera->MoveLocalX(-CAM_MOVE_SPEED * frameTimer);

	if (myEngine->KeyHeld(RIGHT))
		myCamera->MoveLocalX(CAM_MOVE_SPEED * frameTimer);
}

// resets the camera position when looking at a hover car
void MyCamera::resetCamPos(Player_HoverCar* playerHoverCar, IModel* dummy)
{
	if (myEngine->KeyHit(CAM_RESET_FOLLOW))
	{
		myCamera->RotateLocalX(-cameraRotateY);
		myCamera->RotateY(-cameraRotateX);
		cameraRotateY = 0.0f;
		cameraRotateX = 0.0f;
		myCamera->AttachToParent(dummy);		
		myCamera->SetLocalPosition(defaultX, defaultY, defaultZ);
	}

	if (myEngine->KeyHit(CAM_RESET_COCKPIT))
	{
		myCamera->RotateLocalX(-cameraRotateY);
		myCamera->RotateY(-cameraRotateX);
		cameraRotateY = 0.0f;
		cameraRotateX = 0.0f;
		myCamera->AttachToParent(playerHoverCar->getModel());
		myCamera->SetLocalPosition(0.0f, CAM_COCKPIT_YPOS, CAM_COCKPIT_ZPOS);
	}
}