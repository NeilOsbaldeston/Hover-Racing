// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "RaceCourse.h"
#include "Globals.h"

using namespace tle;


// constructor - load up all the required meshes to create the environment
RaceCourse::RaceCourse(I3DEngine* myEngine) : 
	myEngine(myEngine), stageCount(0), startPoint(nullptr)
{
	waypointDummy = myEngine->LoadMesh(DUMMY);
	skyboxMesh = myEngine->LoadMesh(SKYBOX_07);
	groundMesh = myEngine->LoadMesh(GROUND);
	checkPointMesh = myEngine->LoadMesh(CHECK_POINT);
	tankSmall1Mesh = myEngine->LoadMesh(TANK_SMALL_1);
	tankSmall2Mesh = myEngine->LoadMesh(TANK_SMALL_2);
	tribune1Mesh = myEngine->LoadMesh(TRIBUNE_1);
	wallMesh = myEngine->LoadMesh(WALL);
	isleStraightMesh = myEngine->LoadMesh(ISLE_STRAIGHT);
	flareMesh = myEngine->LoadMesh(FLARE);
	treeMesh = myEngine->LoadMesh(TREE);
	garageLargeMesh = myEngine->LoadMesh(GARAGE_LARGE);
	garageSmallMesh = myEngine->LoadMesh(GARAGE_SMALL);
	isleCornerMesh = myEngine->LoadMesh(ISLE_CORNER);
	isleCrossMesh = myEngine->LoadMesh(ISLE_CROSS);
	isleTeeMesh = myEngine->LoadMesh(ISLE_TEE);
	lampMesh = myEngine->LoadMesh(LAMP);
	tankLarge1Mesh = myEngine->LoadMesh(TANK_LARGE_1);
	tankLarge2Mesh = myEngine->LoadMesh(TANK_LARGE_2);
	tribune2Mesh = myEngine->LoadMesh(TRIBUNE_2);
	tribune3Mesh = myEngine->LoadMesh(TRIBUNE_3);
	walkwayMesh = myEngine->LoadMesh(WALKWAY);
}

// destructor
RaceCourse::~RaceCourse()
{
	skyboxMesh->RemoveModel(skybox);
	groundMesh->RemoveModel(ground);

	if (startPoint != nullptr)
		waypointDummy->RemoveModel(startPoint);

	for (auto it = waypointsAI.begin(); it != waypointsAI.end(); ++it)
	{
		if ((*it)->model != nullptr)
			waypointDummy->RemoveModel((*it)->model);
	}
	for (auto it = checkPoints.begin(); it != checkPoints.end(); ++it)
	{
		if ((*it)->model != nullptr)
			checkPointMesh->RemoveModel((*it)->model);
	}
	for (auto it = tankSmall1s.begin(); it != tankSmall1s.end(); ++it)
	{
		if ((*it)->model != nullptr)
			tankSmall1Mesh->RemoveModel((*it)->model);
	}
	for (auto it = tankSmall2s.begin(); it != tankSmall2s.end(); ++it)
	{
		if ((*it)->model != nullptr)
			tankSmall2Mesh->RemoveModel((*it)->model);
	}
	for (auto it = isleStraights.begin(); it != isleStraights.end(); ++it)
	{
		if ((*it)->model != nullptr)
			isleStraightMesh->RemoveModel((*it)->model);
	}
	for (auto it = walls.begin(); it != walls.end(); ++it)
	{
		if ((*it)->model != nullptr)
			wallMesh->RemoveModel((*it)->model);
	}
	for (auto it = tribune1s.begin(); it != tribune1s.end(); ++it)
	{
		if ((*it)->model != nullptr)
			tribune1Mesh->RemoveModel((*it)->model);
	}
	for (auto it = flares.begin(); it != flares.end(); ++it)
	{
		if ((*it)->model != nullptr)
			flareMesh->RemoveModel((*it)->model);
	}
	for (auto it = trees.begin(); it != trees.end(); ++it)
	{
		if ((*it)->model != nullptr)
			treeMesh->RemoveModel((*it)->model);
	}
	for (auto it = garageLarges.begin(); it != garageLarges.end(); ++it)
	{
		if ((*it)->model != nullptr)
			garageLargeMesh->RemoveModel((*it)->model);
	}
	for (auto it = garageSmalls.begin(); it != garageSmalls.end(); ++it)
	{
		if ((*it)->model != nullptr)
			garageSmallMesh->RemoveModel((*it)->model);
	}
	for (auto it = isleCorners.begin(); it != isleCorners.end(); ++it)
	{
		if ((*it)->model != nullptr)
			isleCornerMesh->RemoveModel((*it)->model);
	}
	for (auto it = isleCrosses.begin(); it != isleCrosses.end(); ++it)
	{
		if ((*it)->model != nullptr)
			isleCrossMesh->RemoveModel((*it)->model);
	}
	for (auto it = isleTees.begin(); it != isleTees.end(); ++it)
	{
		if ((*it)->model != nullptr)
			isleTeeMesh->RemoveModel((*it)->model);
	}
	for (auto it = lamps.begin(); it != lamps.end(); ++it)
	{
		if ((*it)->model != nullptr)
			lampMesh->RemoveModel((*it)->model);
	}
	for (auto it = tankLarge1s.begin(); it != tankLarge1s.end(); ++it)
	{
		if ((*it)->model != nullptr)
			tankLarge1Mesh->RemoveModel((*it)->model);
	}
	for (auto it = tankLarge2s.begin(); it != tankLarge2s.end(); ++it)
	{
		if ((*it)->model != nullptr)
			tankLarge2Mesh->RemoveModel((*it)->model);
	}
	for (auto it = tribune2s.begin(); it != tribune2s.end(); ++it)
	{
		if ((*it)->model != nullptr)
			tribune2Mesh->RemoveModel((*it)->model);
	}
	for (auto it = tribune3s.begin(); it != tribune3s.end(); ++it)
	{
		if ((*it)->model != nullptr)
			tribune3Mesh->RemoveModel((*it)->model);
	}
	for (auto it = walkways.begin(); it != walkways.end(); ++it)
	{
		if ((*it)->model != nullptr)
			walkwayMesh->RemoveModel((*it)->model);
	}
}

// opens a file for reading, returns true if successful or returns false if failed
bool RaceCourse::openFile(ifstream& inFile, const string& fileName)
{
	inFile.open(fileName);

	if (!inFile.is_open())
		return false;

	return true;
}

// takes the data from the fstream and returns an array of strings containing each line
string* RaceCourse::extractText(ifstream& inFile, int& lineCount)
{
	lineCount = 0;
	string* container;
	string temp;
	int emptyLineCount = 0;

	while (getline(inFile, temp))
	{
		if (temp == "")
			emptyLineCount++;
		else
			emptyLineCount = 0;

		lineCount++;
	}

	lineCount -= emptyLineCount;

	container = new string[lineCount];

	inFile.clear();
	inFile.seekg(0);

	for (int i = 0; i < lineCount; ++i)
	{
		getline(inFile, container[i]);
	}

	inFile.close();

	return container;
}

// create the ground and sky
void RaceCourse::createWorld(I3DEngine* myEngine, float skyX, float skyY, float skyZ, float groundX, float groundY, float groundZ)
{
	skybox = skyboxMesh->CreateModel(skyX, skyY, skyZ);
	ground = groundMesh->CreateModel(groundX, groundY, groundZ);
}

// create a track from a text file
void RaceCourse::createTrack(const string& trackFile)
{
	string* trackData;
	int dataLines;

	if (openFile(inFile, trackFile))
	{
		trackData = extractText(inFile, dataLines);
	}

	// for each line in the text file search for key words
	for (int i = 0; i < dataLines; ++i)
	{
		if (trackData[i] == START_POINT)
		{
			createStartPoint(startPoint, waypointDummy, trackData, dataLines, i);
		}
		else if (trackData[i] == WAYPOINT_AI)
		{
			Sphere sphere(nullptr, 0.0f, AI_WAYPOINT_RADIUS2);
			createModels(waypointsAI, waypointDummy, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == CHECK_POINT)
		{
			Sphere sphere( nullptr, 0.0f, CHECKPOINT_PILLAR_RADIUS2);
			stageCount = createModels(checkPoints, checkPointMesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == ISLE_STRAIGHT)
		{
			Box box( nullptr, 0.0f, ISLE_STRAIGHT_WIDTH, ISLE_STRAIGHT_LENGTH );
			createModels(isleStraights, isleStraightMesh, trackData, dataLines, i, &box);
		}
		else if (trackData[i] == WALL)
		{
			Box box( nullptr, 0.0f, WALL_WIDTH, WALL_LENGTH );
			createModels(walls, wallMesh, trackData, dataLines, i, &box);
		}
		else if (trackData[i] == TANK_SMALL_1)
		{
			Sphere sphere( nullptr, 0.0f, TANK_SMALL_1_RADIUS2 );
			createModels(tankSmall1s, tankSmall1Mesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == TANK_SMALL_2)
		{
			Sphere sphere( nullptr, 0.0f, TANK_SMALL_2_RADIUS2 );
			createModels(tankSmall2s, tankSmall2Mesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == TRIBUNE_1)
		{
			Sphere sphere( nullptr, 0.0f, TRIBUNE_1_RADIUS2 );
			createModels(tribune1s, tribune1Mesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == FLARE)
		{
			Sphere sphere( nullptr, 0.0f, FLARE_RADIUS2 );
			createModels(flares, flareMesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == TREE)
		{
			Sphere sphere(nullptr, 0.0f, TREE_RADIUS2);
			createModels(trees, treeMesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == GARAGE_LARGE)
		{
			Box box(nullptr, 0.0f, GARAGE_LARGE_WIDTH, GARAGE_LARGE_LENGTH);
			createModels(garageLarges, garageLargeMesh, trackData, dataLines, i, &box);
		}
		else if (trackData[i] == GARAGE_SMALL)
		{
			Box box(nullptr, 0.0f, GARAGE_SMALL_WIDTH, GARAGE_SMALL_LENGTH);
			createModels(garageSmalls, garageSmallMesh, trackData, dataLines, i, &box);
		}
		else if (trackData[i] == ISLE_CORNER)
		{
			Sphere sphere(nullptr, 0.0f, ISLE_CORNER_RADIUS2);
			createModels(isleCorners, isleCornerMesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == ISLE_CROSS)
		{
			Sphere sphere(nullptr, 0.0f, ISLE_CROSS_RADIUS2);
			createModels(isleCrosses, isleCrossMesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == ISLE_TEE)
		{
			Sphere sphere(nullptr, 0.0f, ISLE_TEE_RADIUS2);
			createModels(isleTees, isleTeeMesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == LAMP)
		{
			Sphere sphere(nullptr, 0.0f, LAMP_RADIUS2);
			createModels(lamps, lampMesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == TANK_LARGE_1)
		{
			Sphere sphere(nullptr, 0.0f, TANK_LARGE_1_RADIUS2);
			createModels(tankLarge1s, tankLarge1Mesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == TANK_LARGE_2)
		{
			Sphere sphere(nullptr, 0.0f, TANK_LARGE_2_RADIUS2);
			createModels(tankLarge2s, tankLarge2Mesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == TRIBUNE_2)
		{
			Sphere sphere(nullptr, 0.0f, TRIBUNE_2_PILLAR_RADIUS2);
			createModels(tribune2s, tribune2Mesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == TRIBUNE_3)
		{
			Sphere sphere(nullptr, 0.0f, TRIBUNE_3_PILLAR_RADIUS2);
			createModels(tribune3s, tribune3Mesh, trackData, dataLines, i, &sphere);
		}
		else if (trackData[i] == WALKWAY)
		{
			Box box(nullptr, 0.0f, WALKWAY_WIDTH, WALKWAY_LENGTH);
			createModels(walkways, walkwayMesh, trackData, dataLines, i, &box);
		}
	}
}

void RaceCourse::createBombParticles()
{
	// create the bomb particles
	for (Sphere* flare : flares)
	{
		bombs.push_back(new Bomb(myEngine, flare->model));
	}
}

// puts ours object partitions all in 1 place for easy iteration
void RaceCourse::createVectorsOfMaps()
{
	checkPointMaps.push_back(checkPointsMap);
	allBoxMaps.push_back(isleStraightsMap);
	allBoxMaps.push_back(wallsMap);
	allSphereMaps.push_back(tankSmall1sMap);
	allSphereMaps.push_back(tankSmall2sMap);
	allSphereMaps.push_back(tribune1sMap);
	allSphereMaps.push_back(flaresMap);
	allSphereMaps.push_back(treesMap);
	allSphereMaps.push_back(tankLarge1sMap);
	allSphereMaps.push_back(tankLarge2sMap);
	allSphereMaps.push_back(isleCornersMap);
	allSphereMaps.push_back(isleCrossesMap);
	allSphereMaps.push_back(isleTeesMap);
}

// Takes all the vector pair <grid ref, model> arrays and outputs associative arrays of grid references and vectors of items in that grid ref for each model type
void RaceCourse::partitionWorld()
{
	mapModelPositions(&checkPointsMap, &checkPoints);
	mapModelPositions(&isleStraightsMap, &isleStraights);
	mapModelPositions(&wallsMap, &walls);
	mapModelPositions(&tankSmall1sMap, &tankSmall1s);
	mapModelPositions(&tankSmall2sMap, &tankSmall2s);
	mapModelPositions(&tribune1sMap, &tribune1s);
	mapModelPositions(&flaresMap, &flares);
	mapModelPositions(&treesMap, &trees);
	mapModelPositions(&tankLarge1sMap, &tankLarge1s);
	mapModelPositions(&tankLarge2sMap, &tankLarge2s);
	mapModelPositions(&isleCornersMap, &isleCorners);
	mapModelPositions(&isleCrossesMap, &isleCrosses);
	mapModelPositions(&isleTeesMap, &isleTees);

	createVectorsOfMaps();
}

// extract the start point from the text file
void RaceCourse::createStartPoint(IModel*& startPoint, IMesh* mesh, string* trackData, int dataLines, int i)
{
	string::size_type sz1 = 0;	// alias of size_t, holds the index count to the end of the current float from where that float began
	string::size_type sz2 = 0;
	const int DATA_SIZE = 12;	// model matrix size minus the trailing 0s and 1 on the end of each row
	float data[DATA_SIZE];	// holds a single models data

	int j = i + 1;
	if (j < dataLines)
	{
		if (trackData[j] != "")
		{
			try
			{
				// go through each number in the row of the string array and convert it from a string into a float, storing the float
				for (int dataIndex = 0; dataIndex < DATA_SIZE; ++dataIndex)
				{
					data[dataIndex] = std::stof(trackData[j].substr(sz1 += sz2), &sz2);
				}
			}
			catch (exception)
			{
				return;	// skip the data line, don't make the model
			}

			float matrix[4][4];

			// transfer the data array into a matrix to be added to the model vector
			int index = 0;
			for (int rows = 0; rows < 4; ++rows)
			{
				for (int columns = 0; columns < 3; ++columns)
				{
					matrix[rows][columns] = data[index];
					index++;
				}
			}

			startPoint = mesh->CreateModel();
			startPoint->SetMatrix(&matrix[0][0]);
		}
	}
}
//