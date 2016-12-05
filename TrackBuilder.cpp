// Neil Osbaldeston

#include "TrackBuilder.h"
#include "RaceCourse.h"
#include "Extras.h"
#include <conio.h>


// constructor
TrackBuilder::TrackBuilder(I3DEngine* myEngine, string track) :
	GameMode(myEngine), myEngine(myEngine), objectTemplate(nullptr), objectDistance(TEMPLATE_START_DISTANCE), nameTemplate(""),
	cameraAngleY(0.0f), cameraAngleZ(0.0f), templateScale(1.0f), saveBox(nullptr), trackSaved(false), savedTimer(0.0f)
{
	// store the track name in the string stream ready to display when saving
	if (track != "")
		trackName << track.substr(TRACK_FILE_PATH.length(), track.length() - TRACK_EXTENSION.length() - TRACK_FILE_PATH.length());

	font = myEngine->LoadFont(UI_FONT, UI_FONT_SIZE);
	UIBorder1 = myEngine->CreateSprite(UI_BORDER);
	UIBorder2 = myEngine->CreateSprite(UI_BORDER, 100.0f);

	// create the mesh for the waypoints and start point in the editor only
	startPointMesh = myEngine->LoadMesh(START_FILE);
	waypointMesh = myEngine->LoadMesh(WAYPOINT_FILE);

	// position build menu
	attachItemsUI();

	if (track != "")
	{
		// create the race course
		raceCourse.createTrack(track);	// make dynamic
		raceCourse.partitionWorld();
	}

	raceCourse.getGround()->SetSkin("SandGrid.png");

	// link the vectors of models into lists so that removal of models can be done
	createLists(raceCourse.getCheckPoints(), checkPointsList);
	createLists(raceCourse.getIsleStraights(), isleStraightsList);
	createLists(raceCourse.getWalls(), wallsList);
	createLists(raceCourse.getTankSmall1s(), tankSmall1sList);
	createLists(raceCourse.getTankSmall2s(), tankSmall2sList);
	createLists(raceCourse.getTribune1s(), tribune1sList);
	createLists(raceCourse.getFlares(), flaresList);
	createLists(raceCourse.getWaypointsAI(), waypointsAIList);
	createLists(raceCourse.getTrees(), treesList);
	createLists(raceCourse.getGarageLarges(), garageLargesList);
	createLists(raceCourse.getGarageSmalls(), garageSmallsList);
	createLists(raceCourse.getIsleCorners(), isleCornersList);
	createLists(raceCourse.getIsleCrosses(), isleCrossesList);
	createLists(raceCourse.getIsleTees(), isleTeesList);
	createLists(raceCourse.getLamps(), lampsList);
	createLists(raceCourse.getTankLarge1s(), tankLarge1sList);
	createLists(raceCourse.getTankLarge2s(), tankLarge2sList);
	createLists(raceCourse.getTribune2s(), tribune2sList);
	createLists(raceCourse.getTribune3s(), tribune3sList);
	createLists(raceCourse.getWalkways(), walkwaysList);
	
	float matrix[4][4];
	// set the radius of the waypoints to the collision size of the balls rather than checkpoints
	for (auto it = waypointsAIList.begin(); it != waypointsAIList.end(); ++it)
	{
		(*it)->model->GetMatrix(&matrix[0][0]);
		raceCourse.getDummyMesh()->RemoveModel((*it)->model);
		(*it)->model = waypointMesh->CreateModel();
		(*it)->model->SetMatrix(&matrix[0][0]);
		(*it)->radius2 = WAYPOINT_RADIUS2;
	}

	// set up our start point so we can interact with it in the editor
	startPoint = new Sphere(nullptr, 1.0f, START_POINT_RADIUS2);

	if (raceCourse.getStartPoint() != nullptr)
	{
		raceCourse.getStartPoint()->GetMatrix(&matrix[0][0]);
		raceCourse.getDummyMesh()->RemoveModel(raceCourse.getStartPoint());
		raceCourse.setStartPoint(nullptr);
		startPoint->model = startPointMesh->CreateModel();
		startPoint->model->SetMatrix(&matrix[0][0]);
	}

	// call to remove any unwanted movement
	myEngine->GetMouseMovementX();	
	myEngine->GetMouseMovementY();
}

// destructor
TrackBuilder::~TrackBuilder()
{
	delete UIBorder1;
	delete UIBorder2;

	for (auto it = UIObjectList.begin(); it != UIObjectList.end(); ++it)
	{
		(*it)->mesh->RemoveModel((*it)->model);
	}
	for (auto it = waypointsAIList.begin(); it != waypointsAIList.end(); ++it)
	{
		waypointMesh->RemoveModel((*it)->model);
		(*it)->model = nullptr;
	}
	for (auto it = raceCourse.getWaypointsAI().begin(); it != raceCourse.getWaypointsAI().end(); ++it)
	{
		if ((*it)->model != nullptr)
			waypointMesh->RemoveModel((*it)->model);
	}

	if (objectTemplate != nullptr)
		meshTemplate->RemoveModel(objectTemplate);

	if (startPoint->model != nullptr)
	{
		startPointMesh->RemoveModel(startPoint->model);
		startPoint->model = nullptr;
	}
}

// attach and position the build menu to the camera
void TrackBuilder::attachItemsUI()
{
	// position corrections for fullscreen not taking into account the titlebar and borders missing
	int posCorX = 0;
	int posCorY = 0;
	if (FULLSCREEN)
	{
		posCorX = BORDER_CORRECTION;
		posCorY = TITLEBAR_CORRECTION;
	}

	// create the UI objects and set their bounding boxes
	int segment = myEngine->GetHeight() / 10;

	int xPos = UI_FIRST_COLUMN_POS - posCorX;

	startingPoint = { START_POINT, startPointMesh, startPointMesh->CreateModel(), xPos, segment - posCorY, UI_HIT_BOX };
	AIwaypoint = { WAYPOINT_AI, waypointMesh, waypointMesh->CreateModel(), xPos, segment * 2 - posCorY, UI_HIT_BOX };
	wall = { WALL, raceCourse.getWallMesh(), raceCourse.getWallMesh()->CreateModel(), xPos, segment * 3 - posCorY, UI_HIT_BOX };
	isleStraight = { ISLE_STRAIGHT, raceCourse.getIsleStraightMesh(), raceCourse.getIsleStraightMesh()->CreateModel(), xPos, segment * 4 - posCorY, UI_HIT_BOX };
	checkPoint = { CHECK_POINT, raceCourse.getCheckPointMesh(), raceCourse.getCheckPointMesh()->CreateModel(), xPos, segment * 5 - posCorY, UI_HIT_BOX };
	tankSmall1 = { TANK_SMALL_1, raceCourse.getTankSmall1Mesh(), raceCourse.getTankSmall1Mesh()->CreateModel(), xPos, segment * 6 - posCorY, UI_HIT_BOX };
	tankSmall2 = { TANK_SMALL_2, raceCourse.getTankSmall2Mesh(), raceCourse.getTankSmall2Mesh()->CreateModel(), xPos, segment * 7 - posCorY, UI_HIT_BOX };
	tribune1 = { TRIBUNE_1, raceCourse.getTribune1Mesh(), raceCourse.getTribune1Mesh()->CreateModel(), xPos, segment * 8 - posCorY, UI_HIT_BOX };
	flare = { FLARE, raceCourse.getFlareMesh(), raceCourse.getFlareMesh()->CreateModel(), xPos, segment * 9 - posCorY, UI_HIT_BOX };

	xPos = UI_SECOND_COLUMN_POS - posCorX;

	tree = { TREE, raceCourse.getTreeMesh(), raceCourse.getTreeMesh()->CreateModel(), xPos, segment - posCorY, UI_HIT_BOX };
	garageLarge = { GARAGE_LARGE, raceCourse.getGarageLargeMesh(), raceCourse.getGarageLargeMesh()->CreateModel(), xPos, segment * 2 - posCorY, UI_HIT_BOX };
	garageSmall = { GARAGE_SMALL, raceCourse.getGarageSmallMesh(), raceCourse.getGarageSmallMesh()->CreateModel(), xPos, segment * 3 - posCorY, UI_HIT_BOX };
	isleCorner = { ISLE_CORNER, raceCourse.getIsleCornerMesh(), raceCourse.getIsleCornerMesh()->CreateModel(), xPos, segment * 4 - posCorY, UI_HIT_BOX };
	isleCross = { ISLE_CROSS, raceCourse.getIsleCrossMesh(), raceCourse.getIsleCrossMesh()->CreateModel(), xPos, segment * 5 - posCorY, UI_HIT_BOX };
	isleTee = { ISLE_TEE, raceCourse.getIsleTeeMesh(), raceCourse.getIsleTeeMesh()->CreateModel(), xPos, segment * 6 - posCorY, UI_HIT_BOX };
	lamp = { LAMP, raceCourse.getLampMesh(), raceCourse.getLampMesh()->CreateModel(), xPos, segment * 7 - posCorY, UI_HIT_BOX };
	tankLarge1 = { TANK_LARGE_1, raceCourse.getTankLarge1Mesh(), raceCourse.getTankLarge1Mesh()->CreateModel(), xPos, segment * 8 - posCorY, UI_HIT_BOX };
	tankLarge2 = { TANK_LARGE_2, raceCourse.getTankLarge2Mesh(), raceCourse.getTankLarge2Mesh()->CreateModel(), xPos, segment * 9 - posCorY, UI_HIT_BOX };

	xPos = UI_THIRD_COLUMN_POS - posCorX;

	tribune2 = { TRIBUNE_2, raceCourse.getTribune2Mesh(), raceCourse.getTribune2Mesh()->CreateModel(), xPos, segment - posCorY, UI_HIT_BOX };
	tribune3 = { TRIBUNE_3, raceCourse.getTribune3Mesh(), raceCourse.getTribune3Mesh()->CreateModel(), xPos, segment * 2 - posCorY, UI_HIT_BOX };
	walkway = { WALKWAY, raceCourse.getWalkwayMesh(), raceCourse.getWalkwayMesh()->CreateModel(), xPos, segment * 3 - posCorY, UI_HIT_BOX };

	// put all UI objects into a vector
	UIObjectList.push_back(&startingPoint);
	UIObjectList.push_back(&AIwaypoint);
	UIObjectList.push_back(&wall);
	UIObjectList.push_back(&isleStraight);
	UIObjectList.push_back(&checkPoint);
	UIObjectList.push_back(&tankSmall1);
	UIObjectList.push_back(&tankSmall2);
	UIObjectList.push_back(&tribune1);
	UIObjectList.push_back(&flare);

	UIObjectList.push_back(&tree);
	UIObjectList.push_back(&garageLarge);
	UIObjectList.push_back(&garageSmall);
	UIObjectList.push_back(&isleCorner);
	UIObjectList.push_back(&isleCross);
	UIObjectList.push_back(&isleTee);
	UIObjectList.push_back(&lamp);
	UIObjectList.push_back(&tankLarge1);
	UIObjectList.push_back(&tankLarge2);

	UIObjectList.push_back(&tribune2);
	UIObjectList.push_back(&tribune3);
	UIObjectList.push_back(&walkway);

	// scale objects to appear the same size (and to fit properly)
	startingPoint.model->Scale(0.0055f);
	AIwaypoint.model->Scale(0.003f);
	wall.model->Scale(0.01f);
	isleStraight.model->Scale(0.008f);
	checkPoint.model->Scale(0.006f);
	tankSmall1.model->Scale(0.005f);
	tankSmall2.model->Scale(0.005f);
	tribune1.model->Scale(0.0013f);
	flare.model->Scale(0.02f);

	tree.model->Scale(0.012f);
	garageLarge.model->Scale(0.0035f);
	garageSmall.model->Scale(0.007f);
	isleCorner.model->Scale(0.007f);
	isleCross.model->Scale(0.0062f);
	isleTee.model->Scale(0.0062f);
	lamp.model->Scale(0.009f);
	tankLarge1.model->Scale(0.0035f);
	tankLarge2.model->Scale(0.0035f);

	tribune2.model->Scale(0.0015f);
	tribune3.model->Scale(0.0013f);
	walkway.model->Scale(0.015f);

	// rotate any that need it
	startingPoint.model->RotateX(-40.0f);
	checkPoint.model->RotateY(30.0f);
	garageLarge.model->RotateY(160.0f);
	garageLarge.model->RotateLocalX(20.0f);
	garageSmall.model->RotateY(160.0f);
	garageSmall.model->RotateLocalX(20.0f);
	isleStraight.model->RotateLocalX(-45.0f);
	isleCorner.model->RotateLocalX(-45.0f);
	isleCross.model->RotateLocalX(-45.0f);
	isleTee.model->RotateLocalX(-45.0f);
	tribune2.model->RotateY(-170.0f);
	tribune2.model->RotateLocalX(25.0f);
	tribune3.model->RotateY(180.0f);
	tribune3.model->RotateLocalX(25.0f);
	walkway.model->RotateLocalX(-45.0f);

	// attach to camera
	startingPoint.model->AttachToParent(myCamera.getModel());
	AIwaypoint.model->AttachToParent(myCamera.getModel());
	wall.model->AttachToParent(myCamera.getModel());
	isleStraight.model->AttachToParent(myCamera.getModel());
	checkPoint.model->AttachToParent(myCamera.getModel());
	tankSmall1.model->AttachToParent(myCamera.getModel());
	tankSmall2.model->AttachToParent(myCamera.getModel());
	tribune1.model->AttachToParent(myCamera.getModel());
	flare.model->AttachToParent(myCamera.getModel());

	tree.model->AttachToParent(myCamera.getModel());
	garageLarge.model->AttachToParent(myCamera.getModel());
	garageSmall.model->AttachToParent(myCamera.getModel());
	isleCorner.model->AttachToParent(myCamera.getModel());
	isleCross.model->AttachToParent(myCamera.getModel());
	isleTee.model->AttachToParent(myCamera.getModel());
	lamp.model->AttachToParent(myCamera.getModel());
	tankLarge1.model->AttachToParent(myCamera.getModel());
	tankLarge2.model->AttachToParent(myCamera.getModel());

	tribune2.model->AttachToParent(myCamera.getModel());
	tribune3.model->AttachToParent(myCamera.getModel());
	walkway.model->AttachToParent(myCamera.getModel());

	// set position on the screen
	startingPoint.model->SetLocalPosition(-0.79f, 0.40f, 1.0f);
	AIwaypoint.model->SetLocalPosition(-0.79f, 0.3f, 1.0f);
	wall.model->SetLocalPosition(-0.8f, 0.18f, 1.0f);
	isleStraight.model->SetLocalPosition(-0.78f, 0.077f, 1.0f);
	checkPoint.model->SetLocalPosition(-0.795f, -0.04f, 1.0f);
	tankSmall1.model->SetLocalPosition(-0.8f, -0.135f, 1.0f);
	tankSmall2.model->SetLocalPosition(-0.8f, -0.235f, 1.0f);
	tribune1.model->SetLocalPosition(-0.795f, -0.34f, 1.0f);
	flare.model->SetLocalPosition(-0.797f, -0.395f, 1.0f);

	tree.model->SetLocalPosition(-0.71f, 0.36f, 1.0f);
	garageLarge.model->SetLocalPosition(-0.7f, 0.28f, 1.0f);
	garageSmall.model->SetLocalPosition(-0.7f, 0.18f, 1.0f);
	isleCorner.model->SetLocalPosition(-0.7f, 0.077f, 1.0f);
	isleCross.model->SetLocalPosition(-0.69f, -0.01f, 1.0f);
	isleTee.model->SetLocalPosition(-0.69f, -0.108f, 1.0f);
	lamp.model->SetLocalPosition(-0.7f, -0.2f, 1.0f);
	tankLarge1.model->SetLocalPosition(-0.7f, -0.33f, 1.0f);
	tankLarge2.model->SetLocalPosition(-0.7f, -0.43f, 1.0f);

	tribune2.model->SetLocalPosition(-0.59f, 0.36f, 1.0f);
	tribune3.model->SetLocalPosition(-0.6f, 0.265f, 1.0f);
	walkway.model->SetLocalPosition(-0.61f, 0.20f, 1.0f);
}

// clicking on the track items selects which type to place
void TrackBuilder::selectTemplate()
{
	bool overUI = false;
	for (auto it = UIObjectList.begin(); it != UIObjectList.end(); ++it)
	{
		if (myEngine->GetMouseX() < (*it)->xPos + (*it)->size && myEngine->GetMouseX() > (*it)->xPos - (*it)->size &&
			myEngine->GetMouseY() < (*it)->yPos + (*it)->size && myEngine->GetMouseY() > (*it)->yPos - (*it)->size)
		{
			overUI = true;

			if (myEngine->KeyHit(SELECT_PLACE))
			{
				// if there is already a template in use, delete it
				if (objectTemplate != nullptr)
				{
					meshTemplate->RemoveModel(objectTemplate);
				}

				// create the new template and attach it to the camera
				meshTemplate = (*it)->mesh;
				objectTemplate = meshTemplate->CreateModel();
				nameTemplate = (*it)->name;
				objectTemplate->AttachToParent(myCamera.getModel());
				objectTemplate->SetLocalPosition(0.0f, 0.0f, objectDistance);

				// reset the scaling var for the new template
				templateScale = 1.0f;

				// set adjusted scaled
				if (nameTemplate == TREE)
				{
					objectTemplate->Scale(4.0f);
					templateScale = 4.0f;
				}	
				else if (nameTemplate == GARAGE_LARGE)
				{
					objectTemplate->Scale(10.0f);
					templateScale = 10.0f;
				}
				else if (nameTemplate == GARAGE_SMALL)
				{
					objectTemplate->Scale(10.0f);
					templateScale = 10.0f;
				}
				else if (nameTemplate == WALKWAY)
				{
					objectTemplate->Scale(5.0f);
					templateScale = 5.0f;
				}		
				else if (nameTemplate == LAMP)
				{
					objectTemplate->Scale(2.0f);
					templateScale = 2.0f;
				}
			}
		}
	}

	if (!overUI)
	{
		createModelFromTemplate();
	}
}

// make the template move with the cursor
void TrackBuilder::moveTemplate(float frameTimer)
{
	float posX = static_cast<float>(myEngine->GetMouseX() - myEngine->GetWidth() / 2) * objectDistance * 0.0009f;
	float posY = static_cast<float>(-myEngine->GetMouseY() + myEngine->GetHeight() / 2) * objectDistance * 0.0009f;

	if (objectTemplate != nullptr)
	{
		objectTemplate->SetLocalPosition(posX, posY, objectDistance);

		// don't allow the model to pass through the floor unless an overide key is pressed
		if (objectTemplate->GetY() < 0.0f && !myEngine->KeyHeld(FLOOR_OVERIDE))
		{
			float camMatrix[4][4];
			myCamera.getModel()->GetMatrix(&camMatrix[0][0]);

			// get the length of the cameras Y vector
			float vecYx = camMatrix[1][0];
			float vecYy = camMatrix[1][1];
			float vecYz = camMatrix[1][2];

			float length = sqrt(vecYx * vecYx + vecYy * vecYy + vecYz * vecYz);

			//normalise the Y vector (only need the y portion)
			vecYx /= length;
			vecYy /= length;
			vecYz /= length;

			// get the angle between the cam Y vector and the world Y axis
			float alpha = acos(vecYy);
			cameraAngleY = alpha * 180.0f / PI;

			// position the object template so that it is level with the floor but still facing the direction of the camera
			objectTemplate->ResetOrientation();
			objectTemplate->SetY(0.0f);
			objectTemplate->RotateLocalX(-cameraAngleY);
		}
		else
		{ 
			// the template keeps its orientation in the ZY plane
			objectTemplate->RotateLocalX(myCamera.getCameraRotateX());
		}
	}
	myCamera.setCamRotationX(0.0f);
	myCamera.setCamRotationY(0.0f);

	if (myEngine->KeyHeld(MOVE_OUT))
	{
		objectDistance += MOVE_DISTANCE_SPEED * frameTimer;
	}
	else if (myEngine->KeyHeld(MOVE_IN))
	{
		objectDistance -= MOVE_DISTANCE_SPEED * frameTimer;
	}
}

// rotations are fixed to world axis X and Y
void TrackBuilder::rotateTemplate(float frameTimer)
{
	if (myEngine->KeyHeld(ROTATE_LEFT))
	{
		objectTemplate->RotateY(-TEMPLATE_ROTATION_SPEED * frameTimer);
	}
	else if (myEngine->KeyHeld(ROTATE_RIGHT))
	{
		objectTemplate->RotateY(TEMPLATE_ROTATION_SPEED * frameTimer);
	}
	else if (myEngine->KeyHeld(ROTATE_UP))
	{
		objectTemplate->RotateX(-TEMPLATE_ROTATION_SPEED * frameTimer);
	}
	else if (myEngine->KeyHeld(ROTATE_DOWN))
	{
		objectTemplate->RotateX(TEMPLATE_ROTATION_SPEED * frameTimer);
	}
}

// change the size of the template
void TrackBuilder::scaleTemplate(float frameTimer)
{
	if (myEngine->KeyHeld(SCALE_UP))
	{
		templateScale += SCALE_SPEED * frameTimer;
		objectTemplate->ResetScale();
		objectTemplate->Scale(templateScale);
	}
	else if (myEngine->KeyHeld(SCALE_DOWN))
	{
		templateScale -= SCALE_SPEED * frameTimer;
		objectTemplate->ResetScale();
		objectTemplate->Scale(templateScale);
	}
}

// left clicking when an item is selected will place that item
void TrackBuilder::createModelFromTemplate()
{
	if (objectTemplate != nullptr)
	{
		if (myEngine->KeyHit(SELECT_PLACE) || myEngine->KeyHit(PLACE_MODEL))
		{
			float matrix[4][4];
			objectTemplate->GetMatrix(&matrix[0][0]);

			float camMatrix[4][4];
			myCamera.getModel()->GetMatrix(&camMatrix[0][0]);

			// get the length of the cameras Z vector
			float vecZx = camMatrix[2][0];
			float vecZy = camMatrix[2][1];
			float vecZz = camMatrix[2][2];

			float length = sqrt(vecZx * vecZx + vecZy * vecZy + vecZz * vecZz);

			//normalise the Z vector
			vecZx /= length;
			vecZy /= length;
			vecZz /= length;

			// get the angle between the cam Z vector and the world Z axis (only need z component as comparing with world Z axis)
			float alpha = acos(vecZz);
			cameraAngleZ = alpha * 180.0f / PI;

			// get direction of angle
			float crossY = -vecZx;

			// find the direction the camera is facing compared to the world Z axis and adjust the angle
			if (crossY > 0.0f)
				cameraAngleZ = -cameraAngleZ;

			///////////////////////////////////////////////////////////////////////////////////////////

			//// get the length of the cameras Y vector
			//float vecYx = camMatrix[1][0];
			//float vecYy = camMatrix[1][1];
			//float vecYz = camMatrix[1][2];

			//length = sqrt(vecYx * vecYx + vecYy * vecYy + vecYz * vecYz);

			////normalise the Y vector (only need the y portion)
			//vecYx /= length;
			//vecYy /= length;
			//vecYz /= length;

			//// get the angle between the cam Y vector and the world Y axis
			//alpha = acos(vecYy);
			//cameraAngleY = alpha * 180.0f / PI;

			//// get direction of angle
			//float crossX = -vecYx;

			//// find the direction the camera is facing compared to the world Z axis and adjust the angle
			//if (crossX < 0.0f)
			//	cameraAngleY = -cameraAngleY;

			///////////////////////////////////////////////////////////////////////////////////////////

			// position the object in the same orientation as the camera within the cameras local space
			//rotateXandOrbit(matrix, -cameraAngleY);
			rotateYandOrbit(matrix, cameraAngleZ);	

			// add the cameras world point to the objects local point (to the camera) to give the object a world location
			matrix[3][0] += camMatrix[3][0];
			matrix[3][1] += camMatrix[3][1];
			matrix[3][2] += camMatrix[3][2];

			// check the name of the template and create a new model, setting its matrix from the templates, 
			// then adding the new model to the raceCourse model list
			if (nameTemplate == START_POINT)
			{
				// there can be only 1 start point, so we check if theres already a model stored, if there is we remove it and add the new one
				IModel* object = startPointMesh->CreateModel();
				object->SetMatrix(&matrix[0][0]);

				if (startPoint->model != nullptr)
					startPointMesh->RemoveModel(startPoint->model);

				startPoint->model = object;
			}
			else if (nameTemplate == WAYPOINT_AI)
			{
				IModel* object = waypointMesh->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, WAYPOINT_RADIUS2);
				waypointsAIList.push_back(sphere);
				raceCourse.getWaypointsAI().push_back(sphere);
			}
			else if (nameTemplate == WALL)
			{
				IModel* object = raceCourse.getWallMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Box* box = new Box(object, 0.0f, WALL_WIDTH, WALL_LENGTH);
				wallsList.push_back(box);
				raceCourse.getWalls().push_back(box);
			}
			else if (nameTemplate == ISLE_STRAIGHT)
			{
				IModel* object = raceCourse.getIsleStraightMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Box* box = new Box(object, 0.0f, ISLE_STRAIGHT_WIDTH, ISLE_STRAIGHT_LENGTH);
				isleStraightsList.push_back(box);
				raceCourse.getIsleStraights().push_back(box);
			}
			else if (nameTemplate == CHECK_POINT)
			{
				IModel* object = raceCourse.getCheckPointMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, CHECKPOINT_RADIUS2);
				checkPointsList.push_back(sphere);
				raceCourse.getCheckPoints().push_back(sphere);
			}
			else if (nameTemplate == TANK_SMALL_1)
			{
				IModel* object = raceCourse.getTankSmall1Mesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, TANK_SMALL_1_RADIUS2);
				tankSmall1sList.push_back(sphere);
				raceCourse.getTankSmall1s().push_back(sphere);
			}
			else if (nameTemplate == TANK_SMALL_2)
			{
				IModel* object = raceCourse.getTankSmall2Mesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, TANK_SMALL_2_RADIUS2);
				tankSmall2sList.push_back(sphere);
				raceCourse.getTankSmall2s().push_back(sphere);
			}
			else if (nameTemplate == TRIBUNE_1)
			{
				IModel* object = raceCourse.getTribune1Mesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, TRIBUNE_1_RADIUS2);
				tribune1sList.push_back(sphere);
				raceCourse.getTribune1s().push_back(sphere);
			}
			else if (nameTemplate == FLARE)
			{
				IModel* object = raceCourse.getFlareMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, FLARE_RADIUS2);
				flaresList.push_back(sphere);
				raceCourse.getFlares().push_back(sphere);
			}
			else if (nameTemplate == TREE)
			{
				IModel* object = raceCourse.getTreeMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, TREE_RADIUS2);
				treesList.push_back(sphere);
				raceCourse.getTrees().push_back(sphere);
			}
			else if (nameTemplate == GARAGE_LARGE)
			{
				IModel* object = raceCourse.getGarageLargeMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Box* box = new Box(object, 0.0f, GARAGE_LARGE_WIDTH, GARAGE_LARGE_LENGTH);
				garageLargesList.push_back(box);
				raceCourse.getGarageLarges().push_back(box);
			}
			else if (nameTemplate == GARAGE_SMALL)
			{
				IModel* object = raceCourse.getGarageSmallMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Box* box = new Box(object, 0.0f, GARAGE_SMALL_WIDTH, GARAGE_SMALL_LENGTH);
				garageSmallsList.push_back(box);
				raceCourse.getGarageSmalls().push_back(box);
			}
			else if (nameTemplate == ISLE_CORNER)
			{
				IModel* object = raceCourse.getIsleCornerMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, ISLE_CORNER_RADIUS2);
				isleCornersList.push_back(sphere);
				raceCourse.getIsleCorners().push_back(sphere);
			}
			else if (nameTemplate == ISLE_CROSS)
			{
				IModel* object = raceCourse.getIsleCrossMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, ISLE_CROSS_RADIUS2);
				isleCrossesList.push_back(sphere);
				raceCourse.getIsleCrosses().push_back(sphere);
			}
			else if (nameTemplate == ISLE_TEE)
			{
				IModel* object = raceCourse.getIsleTeeMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, ISLE_TEE_RADIUS2);
				isleTeesList.push_back(sphere);
				raceCourse.getIsleTees().push_back(sphere);
			}
			else if (nameTemplate == LAMP)
			{
				IModel* object = raceCourse.getLampMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, LAMP_RADIUS2);
				lampsList.push_back(sphere);
				raceCourse.getLamps().push_back(sphere);
			}
			else if (nameTemplate == TANK_LARGE_1)
			{
				IModel* object = raceCourse.getTankLarge1Mesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, TANK_LARGE_1_RADIUS2);
				tankLarge1sList.push_back(sphere);
				raceCourse.getTankLarge1s().push_back(sphere);
			}
			else if (nameTemplate == TANK_LARGE_2)
			{
				IModel* object = raceCourse.getTankLarge2Mesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, TANK_LARGE_2_RADIUS2);
				tankLarge2sList.push_back(sphere);
				raceCourse.getTankLarge2s().push_back(sphere);
			}
			else if (nameTemplate == TRIBUNE_2)
			{
				IModel* object = raceCourse.getTribune2Mesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, TRIBUNE_2_PILLAR_RADIUS2);
				tribune2sList.push_back(sphere);
				raceCourse.getTribune2s().push_back(sphere);
			}
			else if (nameTemplate == TRIBUNE_3)
			{
				IModel* object = raceCourse.getTribune3Mesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Sphere* sphere = new Sphere(object, 0.0f, TRIBUNE_3_PILLAR_RADIUS2);
				tribune3sList.push_back(sphere);
				raceCourse.getTribune3s().push_back(sphere);
			}
			else if (nameTemplate == WALKWAY)
			{
				IModel* object = raceCourse.getWalkwayMesh()->CreateModel();
				object->SetMatrix(&matrix[0][0]);
				Box* box = new Box(object, 0.0f, WALKWAY_WIDTH, WALKWAY_LENGTH);
				walkwaysList.push_back(box);
				raceCourse.getWalkways().push_back(box);
			}
		}
	}
}

// remove a model from the scene
void TrackBuilder::deleteModel()
{
	if (!deleteBox(wallsList, raceCourse.getWallMesh()))
		if (!deleteBox(isleStraightsList, raceCourse.getIsleStraightMesh()))
			if (!deleteSphere(checkPointsList, raceCourse.getCheckPointMesh()))
				if (!deleteSphere(tankSmall1sList, raceCourse.getTankSmall1Mesh()))
					if (!deleteSphere(tankSmall2sList, raceCourse.getTankSmall2Mesh()))
						if (!deleteSphere(tribune1sList, raceCourse.getTribune1Mesh()))
							if (!deleteSphere(flaresList, raceCourse.getFlareMesh()))
								if (!deleteSphere(waypointsAIList, waypointMesh))
									if (!deleteSphere(treesList, raceCourse.getTreeMesh()))
										if (!deleteBox(garageLargesList, raceCourse.getGarageLargeMesh()))
											if (!deleteBox(garageSmallsList, raceCourse.getGarageSmallMesh()))
												if (!deleteSphere(isleCornersList, raceCourse.getIsleCornerMesh()))
													if (!deleteSphere(isleCrossesList, raceCourse.getIsleCrossMesh()))
														if (!deleteSphere(isleTeesList, raceCourse.getIsleTeeMesh()))
															if (!deleteSphere(lampsList, raceCourse.getLampMesh()))
																if (!deleteSphere(tankLarge1sList, raceCourse.getTankLarge1Mesh()))
																	if (!deleteSphere(tankLarge2sList, raceCourse.getTankLarge2Mesh()))
																		if (!deleteSphere(tribune2sList, raceCourse.getTribune2Mesh()))
																			if (!deleteSphere(tribune3sList, raceCourse.getTribune3Mesh()))
																				deleteBox(walkwaysList, raceCourse.getWalkwayMesh());
}

// right clicking over a placed item will remove it (must hold for 2 second)
bool TrackBuilder::deleteSphere(list<Sphere*>& modelList, IMesh* mesh)
{
	Sphere* temp = nullptr;

	for (auto it = modelList.begin(); it != modelList.end(); ++it)
	{
		float radius = sqrtf((*it)->radius2);
		if (objectTemplate->GetX() < (*it)->model->GetX() + radius &&
			objectTemplate->GetX() > (*it)->model->GetX() - radius &&
			objectTemplate->GetY() < (*it)->model->GetY() + radius &&
			objectTemplate->GetY() > (*it)->model->GetY() - radius &&
			objectTemplate->GetZ() < (*it)->model->GetZ() + radius &&
			objectTemplate->GetZ() > (*it)->model->GetZ() - radius)
		{
			temp = *it;
			mesh->RemoveModel(temp->model);
			(*it)->model = nullptr;
			break;
		}		
	}
		
	if (temp != nullptr)
	{
		modelList.remove(temp);
		return true;
	}
	return false;
}

// right clicking over a placed item will remove it (must hold for 2 second)
bool TrackBuilder::deleteBox(list<Box*>& modelList, IMesh* mesh)
{
	Box* temp = nullptr;

	for (auto it = modelList.begin(); it != modelList.end(); ++it)
	{
		float radius = (*it)->width / 2.0f;
		if (objectTemplate->GetX() < (*it)->model->GetX() + radius &&
			objectTemplate->GetX() > (*it)->model->GetX() - radius &&
			objectTemplate->GetY() < (*it)->model->GetY() + radius &&
			objectTemplate->GetY() > (*it)->model->GetY() - radius &&
			objectTemplate->GetZ() < (*it)->model->GetZ() + radius &&
			objectTemplate->GetZ() > (*it)->model->GetZ() - radius)
		{
			temp = *it;
			mesh->RemoveModel(temp->model);
			(*it)->model = nullptr;
			break;
		}
	}

	if (temp != nullptr)
	{
		modelList.remove(temp);
		return true;
	}
	return false;
}

// opens a file for writing, returns true if successful or returns false if failed
bool TrackBuilder::openFile(ofstream& outFile, const string& fileName)
{
	outFile.open(fileName);

	if (!outFile.is_open())
		return false;

	return true;
}

// generate a simple text file containing all the model matrices of the new track design
void TrackBuilder::writeFile(ofstream& outFile)
{
	if (startPoint->model != nullptr)
	{
		float matrix[4][4];
		stringstream stream;

		outFile << START_POINT << '\n';
		startPoint->model->GetMatrix(&matrix[0][0]);
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				stream << matrix[i][j];
				outFile << stream.str() << ' ';
				stream.str("");
			}
			outFile << "   ";
		}
		outFile << '\n' << '\n';
	}

	if (waypointsAIList.size() > 0)
		writeArraysToFile(outFile, waypointsAIList, WAYPOINT_AI);

	if (checkPointsList.size() > 0)
		writeArraysToFile(outFile, checkPointsList, CHECK_POINT);

	if (isleStraightsList.size() > 0)
		writeArraysToFile(outFile, isleStraightsList, ISLE_STRAIGHT);

	if (wallsList.size() > 0)
		writeArraysToFile(outFile, wallsList, WALL);

	if (tankSmall1sList.size() > 0)
		writeArraysToFile(outFile, tankSmall1sList, TANK_SMALL_1);

	if (tankSmall2sList.size() > 0)
		writeArraysToFile(outFile, tankSmall2sList, TANK_SMALL_2);

	if (tribune1sList.size() > 0)
		writeArraysToFile(outFile, tribune1sList, TRIBUNE_1);

	if (flaresList.size() > 0)
		writeArraysToFile(outFile, flaresList, FLARE);

	if (treesList.size() > 0)
		writeArraysToFile(outFile, treesList, TREE);

	if (garageLargesList.size() > 0)
		writeArraysToFile(outFile, garageLargesList, GARAGE_LARGE);

	if (garageSmallsList.size() > 0)
		writeArraysToFile(outFile, garageSmallsList, GARAGE_SMALL);

	if (isleCornersList.size() > 0)
		writeArraysToFile(outFile, isleCornersList, ISLE_CORNER);

	if (isleCrossesList.size() > 0)
		writeArraysToFile(outFile, isleCrossesList, ISLE_CROSS);

	if (isleTeesList.size() > 0)
		writeArraysToFile(outFile, isleTeesList, ISLE_TEE);

	if (lampsList.size() > 0)
		writeArraysToFile(outFile, lampsList, LAMP);

	if (tankLarge1sList.size() > 0)
		writeArraysToFile(outFile, tankLarge1sList, TANK_LARGE_1);

	if (tankLarge2sList.size() > 0)
		writeArraysToFile(outFile, tankLarge2sList, TANK_LARGE_2);

	if (tribune2sList.size() > 0)
		writeArraysToFile(outFile, tribune2sList, TRIBUNE_2);

	if (tribune3sList.size() > 0)
		writeArraysToFile(outFile, tribune3sList, TRIBUNE_3);

	if (walkwaysList.size() > 0)
		writeArraysToFile(outFile, walkwaysList, WALKWAY);	

	outFile.close();
}

// take keyboard input and prints it on the screen, used for the track name when saving
bool TrackBuilder::readInTrackName()
{
	// cancel saving
	if (myEngine->KeyHit(ESCAPE))
	{
		saving = false;
		return false;
	}

	string keyboardEntry = "";

	if (myEngine->KeyHit(Key_Space))
		keyboardEntry = " ";
	else if (myEngine->KeyHit(Key_0))
		keyboardEntry = "0";
	else if (myEngine->KeyHit(Key_1))
		keyboardEntry = "1";
	else if (myEngine->KeyHit(Key_2))
		keyboardEntry = "2";
	else if (myEngine->KeyHit(Key_3))
		keyboardEntry = "3";
	else if (myEngine->KeyHit(Key_4))
		keyboardEntry = "4";
	else if (myEngine->KeyHit(Key_5))
		keyboardEntry = "5";
	else if (myEngine->KeyHit(Key_6))
		keyboardEntry = "6";
	else if (myEngine->KeyHit(Key_7))
		keyboardEntry = "7";
	else if (myEngine->KeyHit(Key_8))
		keyboardEntry = "8";
	else if (myEngine->KeyHit(Key_9))
		keyboardEntry = "9";
	else if (myEngine->KeyHit(Key_A))
		keyboardEntry = "a";
	else if (myEngine->KeyHit(Key_B))
		keyboardEntry = "b";
	else if (myEngine->KeyHit(Key_C))
		keyboardEntry = "c";
	else if (myEngine->KeyHit(Key_D))
		keyboardEntry = "d";
	else if (myEngine->KeyHit(Key_E))
		keyboardEntry = "e";
	else if (myEngine->KeyHit(Key_F))
		keyboardEntry = "f";
	else if (myEngine->KeyHit(Key_G))
		keyboardEntry = "g";
	else if (myEngine->KeyHit(Key_H))
		keyboardEntry = "h";
	else if (myEngine->KeyHit(Key_I))
		keyboardEntry = "i";
	else if (myEngine->KeyHit(Key_J))
		keyboardEntry = "j";
	else if (myEngine->KeyHit(Key_K))
		keyboardEntry = "k";
	else if (myEngine->KeyHit(Key_L))
		keyboardEntry = "l";
	else if (myEngine->KeyHit(Key_M))
		keyboardEntry = "m";
	else if (myEngine->KeyHit(Key_N))
		keyboardEntry = "n";
	else if (myEngine->KeyHit(Key_O))
		keyboardEntry = "o";
	else if (myEngine->KeyHit(Key_P))
		keyboardEntry = "p";
	else if (myEngine->KeyHit(Key_Q))
		keyboardEntry = "q";
	else if (myEngine->KeyHit(Key_R))
		keyboardEntry = "r";
	else if (myEngine->KeyHit(Key_S))
		keyboardEntry = "s";
	else if (myEngine->KeyHit(Key_T))
		keyboardEntry = "t";
	else if (myEngine->KeyHit(Key_U))
		keyboardEntry = "u";
	else if (myEngine->KeyHit(Key_V))
		keyboardEntry = "v";
	else if (myEngine->KeyHit(Key_W))
		keyboardEntry = "w";
	else if (myEngine->KeyHit(Key_X))
		keyboardEntry = "x";
	else if (myEngine->KeyHit(Key_Y))
		keyboardEntry = "y";
	else if (myEngine->KeyHit(Key_Z))
		keyboardEntry = "z";

	// when enter is pressed, the track is saved but only if there is at least 1 char.
	// We look at the next char to be taken from the stringstream without removing it to 
	// check if there is one
	else if (myEngine->KeyHit(Key_Return) && trackName.peek() != EOF)
	{
		return true;
	}
	// if the next char was the EOF we need need to reset the 
	// error state flags to allow access to the stream again
	trackName.clear();

	// backspace deletes a character
	if (myEngine->KeyHit(Key_Back))
	{
		keyboardEntry = trackName.str();
		trackName.str("");
		if (keyboardEntry.size() > 0)
			keyboardEntry.pop_back();
	}

	// concatenate the latest keyboard entry with the previous entries
	trackName << keyboardEntry;

	// draw the curent name on the screen
	font->Draw(trackName.str(), TRACK_NAME_INPUT_XPOS, TRACK_NAME_INPUT_YPOS);

	return false;
}

// find the length of one of the local axis
float TrackBuilder::getTemplateScale() const
{
	if (objectTemplate != nullptr)
	{
		float matrix[4][4];
		objectTemplate->GetMatrix(&matrix[0][0]);

		// get the length of the X vector
		float vecXx = matrix[1][0];
		float vecXy = matrix[1][1];
		float vecXz = matrix[1][2];

		float length = sqrt(vecXx * vecXx + vecXy * vecXy + vecXz * vecXz);

		return length;
	}

	return 0.0f;
}

// track editor main method, controls all track editor functions
void TrackBuilder::start(float frameTimer, GameStates* gameState, MenuStates* menuState)
{
	if (saving)
	{
		// create one only - the back drop for the track name text
		if (saveBox == nullptr)
			saveBox = myEngine->CreateSprite(SAVE_BOX, static_cast<float>(TRACK_NAME_INPUT_XPOS - 10), 
				static_cast<float>(TRACK_NAME_INPUT_YPOS));

		font->Draw("ENTER THE TRACK'S NAME", TRACK_NAME_INPUT_XPOS + 1, TRACK_NAME_INPUT_YPOS - UI_FONT_SIZE + 1, kWhite);
		font->Draw("ENTER THE TRACK'S NAME", TRACK_NAME_INPUT_XPOS, TRACK_NAME_INPUT_YPOS - UI_FONT_SIZE);

		// returns true when 'Enter' is pressed after typing a track name
		if (readInTrackName())
		{
			string fileName = TRACK_FILE_PATH + trackName.str() + TRACK_EXTENSION;

			if (openFile(outFile, fileName))
				writeFile(outFile);

			saving = false;
			trackSaved = true;
		}
	}
	// if not saving or no longer saving
	else
	{
		// if we previously created a track save box, delete it before continuing
		if (saveBox != nullptr)
		{
			delete saveBox;
			saveBox = nullptr;
		}

		// after a save, 'track saved' text will remain on the screen until the timer reaches a limit
		if (trackSaved && savedTimer < TRACK_SAVED_MAX_TIME)
		{
			savedTimer += frameTimer;
			font->Draw("TRACK SAVED", myEngine->GetWidth() / 2 + 1, myEngine->GetHeight() / 2 + 1, kWhite, kCentre);
			font->Draw("TRACK SAVED", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kBlack, kCentre);
		}
		else
		{
			trackSaved = false;
			savedTimer = 0.0f;
		}

		// operate the track editor
		myCamera.moveCameraEditor(frameTimer, nameTemplate, getTemplateScale());
		selectTemplate();
		moveTemplate(frameTimer);
		rotateTemplate(frameTimer);
		scaleTemplate(frameTimer);
	
		if (myEngine->KeyHit(DELETE_MODEL))
		{
			if (objectTemplate != nullptr)
			{
				deleteModel();
			}
		}
	}
}