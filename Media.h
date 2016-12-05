// Neil Osbaldeston

#pragma once

#include <string>

using namespace std;

//////////////////////
//	Media Folders	//
//////////////////////

const string MEDIA_FOLDERS[] = {
	".\\Media\\Core",
	".\\Media\\Extra"
};


//////////////
//	Objects	//
//////////////

const string START_FILE = "Arrow.x";
const string WAYPOINT_FILE = "Sphere.x";
const string START_IMAGE = "StartPoint.jpg";
const string WAYPOINT_IMAGE = "Waypoint.jpg";

const string START_POINT = "START_POINT";
const string WAYPOINT_AI = "WAYPOINT_AI";
const string SKYBOX_07 = "Skybox 07.x";
const string GROUND = "ground.x";
const string CHECK_POINT = "Checkpoint.x";
const string ISLE_STRAIGHT = "IsleStraight.x";
const string WALL = "Wall.x";	// 9.67 units long
const string TANK_SMALL_1 = "TankSmall1.x";
const string TANK_SMALL_2 = "TankSmall2.x";
const string TRIBUNE_1 = "Tribune1.x";
const string CROSS = "Cross.x";
const string FLARE = "Flare.x";
const string TREE = "plant.x";
const string GARAGE_LARGE = "GarageLarge.x";
const string GARAGE_SMALL = "GarageSmall.x";
const string ISLE_CORNER = "IsleCorner.x";
const string ISLE_CROSS = "IsleCross.x";
const string ISLE_TEE = "IsleTee.x";
const string LAMP = "Lamp.x";
const string TANK_LARGE_1 = "TankLarge1.x";
const string TANK_LARGE_2 = "TankLarge2.x";
const string TRIBUNE_2 = "Tribune2.x";
const string TRIBUNE_3 = "Tribune3.x";
const string WALKWAY = "Walkway.x";

const string RACE_2 = "race2.x";
const string RACE_2_RED = "race2Red.x";
const string DUMMY = "Dummy.x";
const string INTERSTELLAR = "Interstellar.x";


//////////////
//	Sprites	//
//////////////

const string MENU_TITLE = "MenuTitle.png";	// 1000 x 400 pixels
const string UI_BACKDROP = "ui_backdrop.jpg";	// 700 x 60 pixels
const string MENU_BACKDROP = "HDBackdrop.jpg";	// 1280 x 720 pixels
const string UI_BORDER = "UIBorder.png";	// for the track editor
const string RACE_POS_BORDER = "RacePositionBorder.png";
const string PAUSE_OVERLAY = "PauseOverlay.png";
const string SAVE_BOX = "SaveBox.png";

const string BOOSTMETER_EMPTY = "BoostEmpty.png";
const string BOOSTMETER_1 = "Boost1.png";
const string BOOSTMETER_2 = "Boost2.png";
const string BOOSTMETER_3 = "Boost3.png";
const string BOOSTMETER_4 = "Boost4.png";
const string BOOSTMETER_5 = "Boost5.png";
const string BOOSTMETER_6 = "Boost6.png";
const string BOOSTMETER_7 = "Boost7.png";
const string BOOSTMETER_8 = "Boost8.png";
const string BOOSTMETER_9 = "Boost9.png";
const string BOOSTMETER_10 = "Boost10.png";
const string BOOSTMETER_11 = "Boost11.png";
const string BOOSTMETER_12 = "Boost12.png";
const string BOOSTMETER_13 = "Boost13.png";
const string BOOSTMETER_14 = "Boost14.png";

const int BOOSTMETER_WIDTH = 300;
const int BOOSTMETER_HEIGHT = 300;

const float UI_HEIGHT = 60.0f;

const int RACE_POS_BORDER_WIDTH = 624;
const int RACE_POS_BORDER_HEIGHT = 825;

const int PAUSE_OVERLAY_WIDTH = 500;
const int PAUSE_OVERLAY_HEIGHT = 500;


//////////////////
//	Particles	//
//////////////////

const string PARTICLE = "particle.x";
const string FIRE_YELLOW = "FireYellow.tga";
const string FIRE_ORANGE = "FireOrange.tga";
const string BOOST_BLUE = "BoostBlue.tga";


//////////////
//	Fonts	//
//////////////

const string DEBUG_FONT = "Arial";
const string MENU_FONT = "Bauhaus 93";
const string COUNTDOWN_FONT = "Forte";
const string UI_FONT = "Agency FB";
const string TRACKLIST_FONT = "Agency FB";

const int DEBUG_FONT_SIZE = 14;
const int MENU_FONT_SIZE = 40;
const int MENU_FONT_SIZE_LARGE = 60;
const int COUNTDOWN_FONT_SIZE = 120;
const int UI_FONT_SIZE = 50;
const int TRACKLIST_FONT_SIZE = 40;


//////////////
//	Tracks	//
//////////////

const string TRACK_EXTENSION = ".track";
const string TRACK_FILE_PATH = ".\\Tracks\\";	// folder name within parent directory
const char TRACK_FOLDER[] = { ".\\Tracks" };	// char* so that dirent.h can use it


//////////////////
//	Menu Titles	//
//////////////////

const string RACE = "RACE";
const string TIME_TRIAL = "TIME TRIAL";
const string TRACK_BUILDER = "TRACK BUILDER";
const string HIGH_SCORES = "HIGH SCORES";
const string OPTIONS = "OPTIONS";
const string QUIT = "QUIT";

const string NEW = "NEW";
const string EDIT = "EDIT";

const string START = "START";
const string BACK = "BACK";

const string SAVE = "SAVE";
const string RACE_AGAIN = "RACE AGAIN";
const string CONTINUE = "CONTINUE";
const string BACK_TO_MENU = "BACK TO MENU";
const string QUIT_GAME = "QUIT GAME";


//////////////////
//	Menu Other	//
//////////////////

const string TRACK_SELECTION_BOX = "scrollBox.png";
const string SELECTION_BOX = "cycleBox.png";

const int TRACK_BOX_WIDTH = 448;
const int TRACK_BOX_HEIGHT = 201;
const int SELECTION_BOX_WIDTH = 206;
const int SELECTION_BOX_HEIGHT = 80;
const int SEL_BOX_BUTTON_WIDTH = 40;