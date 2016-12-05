// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include "Globals.h"
#include "Media.h"
#include "GameMode.h"
#include "Menu.h"

using namespace tle;


class TrackBuilder : public GameMode
{
private:
	I3DEngine* myEngine;

	IFont* font;

	ISprite* UIBorder1;
	ISprite* UIBorder2;	// second only used until i remake the first bigger
	ISprite* saveBox;

	ofstream outFile;

	// stationary UI models (attached to camera)
	UIBox startingPoint;
	UIBox AIwaypoint;
	UIBox wall;
	UIBox isleStraight;
	UIBox checkPoint;
	UIBox tankSmall1;
	UIBox tankSmall2;
	UIBox tribune1;
	UIBox flare;
	UIBox tree;	// no collision detection, decoration only
	UIBox garageLarge;
	UIBox garageSmall;
	UIBox isleCorner;
	UIBox isleCross;
	UIBox isleTee;
	UIBox lamp;
	UIBox tankLarge1;
	UIBox tankLarge2;
	UIBox tribune2;
	UIBox tribune3;
	UIBox walkway;

	// all the UI models grouped together
	vector<UIBox*> UIObjectList;

	// template details
	IModel* objectTemplate;
	IMesh* meshTemplate;
	string nameTemplate;

	// a new mesh for waypoints and start point since they normally use dummy.x
	IMesh* startPointMesh;
	IMesh* waypointMesh;

	// links to the vector containers in raceCourse (lists for easy removal of any model)
	Sphere* startPoint;
	list<Sphere*> waypointsAIList;
	list<Sphere*> checkPointsList;
	list<Box*> isleStraightsList;
	list<Box*> wallsList;
	list<Sphere*> tankSmall1sList;
	list<Sphere*> tankSmall2sList;
	list<Sphere*> tribune1sList;
	list<Sphere*> flaresList;
	list<Sphere*> treesList;
	list<Box*> garageLargesList;
	list<Box*> garageSmallsList;
	list<Sphere*> isleCornersList;
	list<Sphere*> isleCrossesList;
	list<Sphere*> isleTeesList;
	list<Sphere*> lampsList;
	list<Sphere*> tankLarge1sList;
	list<Sphere*> tankLarge2sList;
	list<Sphere*> tribune2sList;
	list<Sphere*> tribune3sList;
	list<Box*> walkwaysList;

	float cameraAngleY;	// floor counter angle
	float cameraAngleZ;

	float objectDistance;
	float templateScale;

	// track saving
	stringstream trackName;
	bool trackSaved;	// triggers 'track saved' text on the screen for a period
	float savedTimer;	// holds how long the 'track saved' text has been on screen

	/////////////////////////////////////////////////////////////////////////////////

	template<class T>
	void createLists(vector<T*>& modelVec, list<T*>& modelList);

	void attachItemsUI();

	void selectTemplate();
	void moveTemplate(float frameTimer);
	void rotateTemplate(float frameTimer);
	void scaleTemplate(float frameTimer);
	void createModelFromTemplate();
	void deleteModel();
	bool deleteSphere(list<Sphere*>& modelList, IMesh* mesh);
	bool deleteBox(list<Box*>& modelList, IMesh* mesh);

	bool openFile(ofstream& outFile, const string& fileName);
	void writeFile(ofstream& outFile);

	template<class T>
	void writeArraysToFile(ofstream& outFile, list<T*>& modelList, string name);

	bool readInTrackName();

public:
	TrackBuilder(I3DEngine* myEngine, string track = "");

	virtual ~TrackBuilder();

	// getters //

	string getTemplateName() const { return nameTemplate; }
	float getTemplateScale() const;

	// setters //

	virtual void start(float frameTimer, GameStates* gameState, MenuStates* menuState);
};

/*****************************************************************************************/


template<class T>
void TrackBuilder::createLists(vector<T*>& modelVec, list<T*>& modelList)
{
	for (auto it = modelVec.begin(); it != modelVec.end(); ++it)
	{
		modelList.push_back(*it);
	}
}

template<class T>
void TrackBuilder::writeArraysToFile(ofstream& outFile, list<T*>& modelList, string name)
{
	float matrix[4][4];
	stringstream stream;

	outFile << name << '\n';
	for (auto it = modelList.begin(); it != modelList.end(); ++it)
	{
		(*it)->model->GetMatrix(&matrix[0][0]);
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
		outFile << '\n';
	}
	outFile << '\n';
}