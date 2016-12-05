// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <string>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include "Media.h"
#include "Globals.h"
#include "Bomb.h"

using namespace tle;
using namespace std;


class RaceCourse
{
private:
	I3DEngine* myEngine;

	ifstream inFile;

	// object meshes //

	IMesh* waypointDummy;
	IMesh* skyboxMesh;
	IMesh* groundMesh;
	IMesh* checkPointMesh;
	IMesh* isleStraightMesh;
	IMesh* wallMesh;
	IMesh* tankSmall1Mesh;
	IMesh* tankSmall2Mesh;
	IMesh* tribune1Mesh;
	IMesh* flareMesh;
	IMesh* treeMesh;
	IMesh* garageLargeMesh;
	IMesh* garageSmallMesh;
	IMesh* isleCornerMesh;
	IMesh* isleCrossMesh;
	IMesh* isleTeeMesh;
	IMesh* lampMesh;
	IMesh* tankLarge1Mesh;
	IMesh* tankLarge2Mesh;
	IMesh* tribune2Mesh;
	IMesh* tribune3Mesh;
	IMesh* walkwayMesh;


	// object models //

	IModel* skybox;
	IModel* ground;

	IModel* startPoint;

	list<Bomb*> bombs;

	vector<Sphere*> waypointsAI;
	vector<Sphere*> checkPoints;
	vector<Box*> isleStraights;
	vector<Box*> walls;
	vector<Sphere*> tankSmall1s;
	vector<Sphere*> tankSmall2s;
	vector<Sphere*> tribune1s;
	vector<Sphere*> flares;
	vector<Sphere*> trees;
	vector<Box*> garageLarges;
	vector<Box*> garageSmalls;
	vector<Sphere*> isleCorners;
	vector<Sphere*> isleCrosses;
	vector<Sphere*> isleTees;
	vector<Sphere*> lamps;
	vector<Sphere*> tankLarge1s;
	vector<Sphere*> tankLarge2s;
	vector<Sphere*> tribune2s;
	vector<Sphere*> tribune3s;
	vector<Box*> walkways;

	// all objects seperated in to grids
	map<int, vector<Sphere*>> checkPointsMap;
	map<int, vector<Box*>> isleStraightsMap;
	map<int, vector<Box*>> wallsMap;
	map<int, vector<Sphere*>> tankSmall1sMap;
	map<int, vector<Sphere*>> tankSmall2sMap;
	map<int, vector<Sphere*>> tribune1sMap;
	map<int, vector<Sphere*>> flaresMap;
	map<int, vector<Sphere*>> treesMap;
	map<int, vector<Sphere*>> tankLarge1sMap;
	map<int, vector<Sphere*>> tankLarge2sMap;
	map<int, vector<Sphere*>> isleCornersMap;
	map<int, vector<Sphere*>> isleCrossesMap;
	map<int, vector<Sphere*>> isleTeesMap;

	vector<map<int, vector<Sphere*>>> checkPointMaps;
	vector<map<int, vector<Sphere*>>> allSphereMaps;
	vector<map<int, vector<Box*>>> allBoxMaps;

	int stageCount;


	RaceCourse() = delete;		// make sure a default constructor is never created

	bool openFile(ifstream& inFile, const string& fileName);
	string* extractText(ifstream& inFile, int& lineCount);

	template <class T>
	int createModels(vector<T*>& models, IMesh* mesh, string* trackData, int dataLines, int i, T*);

	//void createWaypointsAI(vector<IModel*>& models, IMesh* mesh, string* trackData, int dataLines, int i);
	void createStartPoint(IModel*& models, IMesh* mesh, string* trackData, int dataLines, int i);

	void createVectorsOfMaps();

public:
	// constructor - load up all the required meshes to create the environment
	RaceCourse(I3DEngine* myEngine);

	// destructor
	~RaceCourse();

	// getters //

	IModel* getGround() { return ground; }

	IMesh* getDummyMesh() const { return waypointDummy; }
	IMesh* getCheckPointMesh() const { return checkPointMesh; }
	IMesh* getIsleStraightMesh() const { return isleStraightMesh; }
	IMesh* getWallMesh() const { return wallMesh; }
	IMesh* getTankSmall1Mesh() const { return tankSmall1Mesh; }
	IMesh* getTankSmall2Mesh() const { return tankSmall2Mesh; }
	IMesh* getTribune1Mesh() const { return tribune1Mesh; }
	IMesh* getFlareMesh() const { return flareMesh; }
	IMesh* getTreeMesh() const { return treeMesh; }
	IMesh* getGarageLargeMesh() const { return garageLargeMesh; }
	IMesh* getGarageSmallMesh() const { return garageSmallMesh; }
	IMesh* getIsleCornerMesh() const { return isleCornerMesh; }
	IMesh* getIsleCrossMesh() const { return isleCrossMesh; }
	IMesh* getIsleTeeMesh() const { return isleTeeMesh; }
	IMesh* getLampMesh() const { return lampMesh; }
	IMesh* getTankLarge1Mesh() const { return tankLarge1Mesh; }
	IMesh* getTankLarge2Mesh() const { return tankLarge2Mesh; }
	IMesh* getTribune2Mesh() const { return tribune2Mesh; }
	IMesh* getTribune3Mesh() const { return tribune3Mesh; }
	IMesh* getWalkwayMesh() const { return walkwayMesh; }

	vector<Sphere*>& getWaypointsAI() { return waypointsAI; }
	vector<Sphere*>& getCheckPoints() { return checkPoints; }
	vector<Box*>& getIsleStraights() { return isleStraights; }
	vector<Box*>& getWalls() { return walls; }
	vector<Sphere*>& getTankSmall1s() { return tankSmall1s; }
	vector<Sphere*>& getTankSmall2s() { return tankSmall2s; }
	vector<Sphere*>& getTribune1s() { return tribune1s; }
	vector<Sphere*>& getFlares() { return flares; }
	vector<Sphere*>& getTrees() { return trees; }
	vector<Box*>& getGarageLarges() { return garageLarges; }
	vector<Box*>& getGarageSmalls() { return garageSmalls; }
	vector<Sphere*>& getIsleCorners() { return isleCorners; }
	vector<Sphere*>& getIsleCrosses() { return isleCrosses; }
	vector<Sphere*>& getIsleTees() { return isleTees; }
	vector<Sphere*>& getLamps() { return lamps; }
	vector<Sphere*>& getTankLarge1s() { return tankLarge1s; }
	vector<Sphere*>& getTankLarge2s() { return tankLarge2s; }
	vector<Sphere*>& getTribune2s() { return tribune2s; }
	vector<Sphere*>& getTribune3s() { return tribune3s; }
	vector<Box*>& getWalkways() { return walkways; }

	IModel* getStartPoint() const { return startPoint; };

	vector<map<int, vector<Sphere*>>>& getCheckPointMaps() { return checkPointMaps; }
	vector<map<int, vector<Box*>>>& getBoxObjectMaps() { return allBoxMaps; }
	vector<map<int, vector<Sphere*>>>& getSphereObjectMaps() { return allSphereMaps; }

	// get the bomb particles
	list<Bomb*>& getBombs() { return bombs; }

	int getStageCount() const { return stageCount; };

	// setters //

	void setStartPoint(IModel* model) { startPoint = model; }

	// create the ground and sky
	void createWorld(I3DEngine* myEngine, float skyX = 0.0f, float skyY = 0.0f, float skyZ = 0.0f, float groundX = 0.0f, float groundY = 0.0f, float groundZ = 0.0f);

	// create the race track from a text file
	void createTrack(const string& trackFile);

	void createBombParticles();

	void partitionWorld();

	template <class T>
	void mapModelPositions(map<int, vector<T*>>* mModels, vector<T*>* vModels);
};

/*-----------------------------------------------------------------------------------------------------------------------------------*/

// uses the data retrieved from the text file to construct, position and orientate all the models
template <class T>
int RaceCourse::createModels(vector<T*>& models, IMesh* mesh, string* trackData, int dataLines, int i, T* dataStruct)
{
	string::size_type sz1;	// alias of size_t, holds the index count to the end of the current float from where that float began
	string::size_type sz2;
	const int DATA_SIZE = 12;	// model matrix size minus the trailing 0s and 1 on the end of each row
	float data[DATA_SIZE];	// holds a single models data
	int stageCount = 0;

	for (int j = i + 1; j < dataLines; ++j)
	{
		if (trackData[j] != "")
		{
			sz1 = 0;
			sz2 = 0;

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
				continue;	// skip the data line, don't make the model
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

			// get the length of the objects local X vector to get the overall scale (assuming the object is propertional)
			float scale = sqrtf((matrix[0][0] * matrix[0][0]) + (matrix[0][1] * matrix[0][1]) + (matrix[0][2] * matrix[0][2]));

			IModel* model = mesh->CreateModel();	// moment of creation!!
			model->SetMatrix(&matrix[0][0]);

			// using a user defined copy constructor to do a deep copy of the passed in class
			T* individual = new T(*dataStruct);

			individual->model = model;
			individual->scalar = scale;
			models.push_back(individual);
			stageCount++;
		}
		else
		{
			return stageCount;
		}
	}
	return stageCount;
}

// split up the world / race course into seperate collision zones (only adds models to the collision lists if they are within the grid size)
template <class T>
void RaceCourse::mapModelPositions(map<int, vector<T*>>* mModels, vector<T*>* vModels)
{
	int posX = 0;
	int posZ = 0;

	vector<pair<int, T*>>* vPair = new vector<pair<int, T*>>;

	for (auto it = vModels->begin(); it != vModels->end(); ++it)
	{
		// force all points into a positive position by shifting everything over by half the size of the map in both the X and Z
		// then works out what grid ref they are at. Rows are multiplied by the grid size so that when adding the two together i 
		// get a 1D array representation of a 2D grid
		posX = static_cast<int>(((*it)->model->GetX() + (MAP_GRID_SIZE * SECTOR_SIZE / 2)) / SECTOR_SIZE);	// columns
		posZ = static_cast<int>(((*it)->model->GetZ() + (MAP_GRID_SIZE * SECTOR_SIZE / 2)) / SECTOR_SIZE) * MAP_GRID_SIZE;	// rows

		// store a pointer to each model with its grid ref
		vPair->push_back(make_pair(posX + posZ, *it));
	}

	// iterate though every grid position checking the model list each time, making a new vector containing all 
	// the models at each grid ref, then adds that vector to the map with its grid ref
	for (int z = 0; z < MAP_GRID_SIZE * MAP_GRID_SIZE; z += MAP_GRID_SIZE)
	{
		for (int x = 0; x < MAP_GRID_SIZE; ++x)
		{
			vector< T*> modelVec;
			for (auto iter = vPair->begin(); iter != vPair->end(); ++iter)
			{
				if (iter->first == z + x)
				{
					modelVec.push_back(iter->second);
				}
			}
			mModels->insert(make_pair(z + x, modelVec));
		}
	}
}
