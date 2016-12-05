// Neil Osbaldeston

#pragma once

#include <TL-Engine.h>
using namespace tle;


//////////////
//	States	//
//////////////

enum class GameStates
{
	PLAYING,
	PAUSED,
	MENU
};

enum class RaceStates
{
	NOT_STARTED,
	IN_PROGRESS,
	FINISHED
};

enum class MenuStates
{
	MAIN_MENU,
	RACE_SETUP,
	EDITOR_SETUP,
	HIGH_SCORES,
	OPTIONS
};

enum class CarState
{
	NONE,
	TURNING_LEFT,
	TURNING_RIGHT,
	ACCELERATING_FORWARD,
	ACCELERATING_BACKWARD
};


//////////////////
//	Resolutions	//
//////////////////

const int FULLSCREEN_WIDTH = 1920;
const int FULLSCREEN_HEIGHT = 1080;
const int TITLEBAR_CORRECTION = (FULLSCREEN_HEIGHT / 1000) * 30;
const int BORDER_CORRECTION = FULLSCREEN_HEIGHT / 100;

const int WINDOWED_WIDTH = 1600;
const int WINDOWED_HEIGHT = 900;

const bool FULLSCREEN = false;


//////////////////
// Key bindings //
//////////////////

// system
const EKeyCode ESCAPE = Key_Escape;
const EKeyCode PAUSE = Key_P;
const EKeyCode SELECT = Mouse_LButton;

// car controls
const EKeyCode FORWARD = Key_W;
const EKeyCode BACKWARD = Key_S;
const EKeyCode LEFT = Key_A;
const EKeyCode RIGHT = Key_D;
const EKeyCode BOOST = Key_Space;

// camera controls
const EKeyCode CAM_FORWARD = Key_Up;
const EKeyCode CAM_BACKWARD = Key_Down;
const EKeyCode CAM_LEFT = Key_Left;
const EKeyCode CAM_RIGHT = Key_Right;

const EKeyCode CAM_RESET_FOLLOW = Key_1;
const EKeyCode CAM_RESET_COCKPIT = Key_2;

// race controls

const EKeyCode START_RACE = Key_Space;
const EKeyCode TOGGLE_DEBUG_INFO = Key_F1;

// track builder

const EKeyCode SELECT_PLACE = Mouse_LButton;
const EKeyCode DELETE_MODEL = Mouse_RButton;
const EKeyCode PLACE_MODEL = Key_Return;
const EKeyCode SCALE_UP = Key_Plus;
const EKeyCode SCALE_DOWN = Key_Minus;
const EKeyCode MOVE_OUT = Key_E;
const EKeyCode MOVE_IN = Key_Q;
const EKeyCode FLOOR_OVERIDE = static_cast<EKeyCode>(0x10);	// shift
const EKeyCode WALL_OVERIDE = Key_Tab;	// allows normal camera movement when handling wall pieces
const EKeyCode ROTATE_LEFT = Key_Left;	// template rotation
const EKeyCode ROTATE_RIGHT = Key_Right;	// template rotation
const EKeyCode ROTATE_UP = Key_Up;	// template rotation
const EKeyCode ROTATE_DOWN = Key_Down;	// template rotation


//////////////
//	Menu	//
//////////////

const int MENU_SPACING = 60;	// from the centre of one menu option to the centre of the next
const int MENU_BOX_SPACING = 100;
const int BOX_TEXT_X_OFFSET = 10;
const int BOX_TEXT_Y_OFFSET = 10;

const int TRACKS_SHOWN_MAX = 5;	// how many tracks will be displayed in the track list (can be scrolled up and down through the list)
const int SCROLLBAR_WIDTH = 40;


//////////////////////
//	Object sizes	//
//////////////////////

const int MAP_WIDTH = 2000;	// map will be MAP_WIDTH X MAP_WIDTH
const int MAP_GRID_SIZE = 20;	// map must be MAP_GRID_SIZE X MAP_GRID_SIZE or grid must be big enough to hold the map as a square
const int SECTOR_SIZE = MAP_WIDTH / MAP_GRID_SIZE;

const float WAYPOINT_RADIUS2 = 10.0f;	// used in track editor only
const float START_POINT_RADIUS2 = 10.0f;	// used in track editor only
const float CHECKPOINT_RADIUS2 = 95.0f;	// 9.74 radius squared (stage check only)

const float HOVERCAR_RADIUS = 6.5f / 2.3f;
const float HOVERCAR_RADIUS2 = 42.25f / 2.3f;	// 6.5 radius
const float TANK_SMALL_1_RADIUS2 = 9.86f;	// 3.14 radius
const float TANK_SMALL_2_RADIUS2 = 9.74f;	// 3.12 radius
const float TRIBUNE_1_RADIUS2 = 150.0f;		// 21.85 radius	, 5.0 radius at the bottom
const float FLARE_RADIUS2 = 1.0f;		// 1.0 radius
const float TREE_RADIUS2 = 10.0f;

const float HOVERCAR_WIDTH = 4.46f;
const float HOVERCAR_LENGTH = 12.92f;
const float CHECKPOINT_WIDTH = 17.15f;	// between the centre of each pillar
const float CHECKPOINT_PILLAR_RADIUS2 = 1.65f;	// 1.285 radius 
const float CHECKPOINT_HEIGHT = 10.6f;
const float WALL_WIDTH = 1.868f;
const float WALL_LENGTH = 9.67f;
const float WALL_HEIGHT = 4.52f;
const float ISLE_STRAIGHT_WIDTH = 5.35f;
const float ISLE_STRAIGHT_LENGTH = 6.83f;
const float ISLE_STRAIGHT_HEIGHT = 5.44f;

const float GARAGE_LARGE_WIDTH = 20.0f;
const float GARAGE_LARGE_LENGTH = 13.85f;
const float GARAGE_SMALL_WIDTH = 10.2f;
const float GARAGE_SMALL_LENGTH = 9.85f;
const float ISLE_CORNER_RADIUS2 = 25.0f;	// 5.0f radius
const float ISLE_CROSS_RADIUS2 = 32.49f;	// 5.7f radius
const float ISLE_TEE_RADIUS2 = 25.0f;	// 5.0f radius
const float LAMP_RADIUS2 = 0.593f;	// 0.77f radius
const float TANK_LARGE_1_RADIUS2 = 20.25f;	// 4.5f radius
const float TANK_LARGE_2_RADIUS2 = 16.0f;	// 4.0f radius
const float TRIBUNE_2_PILLAR_RADIUS2 = 25.0f;
const float TRIBUNE_3_PILLAR_RADIUS2 = 25.0f;
const float WALKWAY_WIDTH = 3.2f;
const float WALKWAY_LENGTH = 4.4f;

const float SKYBOX_WIDTH = 2000.0f;
const float SKYBOX_BOUNDARY = 50.0f;	// the size of the gap between the skybox and the cars collision boundary


//////////////////
//	Hover Car	//
//////////////////

const float DEFAULT_START_XPOS = 0.0f;
const float DEFAULT_START_YPOS = 2.7f;
const float DEFAULT_START_ZPOS = 0.0f;
const float START_ANGLE_OFFSET = 10.0f;	// the nose dip

const float MOVE_MULTIPLIER = 150.0f;	// increase the drift vector to a usable amount
const float FORWARD_DRAG = 0.3f;	// decay factor
const float DRIFT_DRAG = 0.3;	// decay factor
const float BOUNCE_DAMPENING = 3.0f;	// reduces the bounce vector by a factor
const float BOUNCE_CAR_DAMPENING = 4.0f;		// reduces the additional bounce vector by a factor

const float FORWARD_ACCELERATION = 1.0f;	// units per second per second
const float BACKWARD_ACCELERATION = 0.6f;	// units per second per second

const float GRAVITY = 5.0f;	// units per second per second
const float LOW_GRAVITY = 1.0f;
const float HOVER_STRENGTH_FULL = 3.0f;	// distance from ground the car can maintain a hover
const float HOVER_STRENGTH_MIN = 15.0f;	// the distance from the ground when the hover engines start to reduce fall speed
const float PORPOISE_CONTROL = 1.015f;	// anti grav strength before dampening is activated (1.0f for completely stable)
const float STRENGTH_MULTI = 6.0f;	// only used to settle the car quicker after falling
const float GRAVITY_CHANGE_MIN = 0.98f;	// min hover strength value at which gravity will be altered to make the car bounce slower
const float GRAVITY_CHANGE_MAX = 1.02f;	// max hover strength value at which gravity will be altered to make the car bounce slower
const float TERMINAL_VELOCITY = 50.0f;	// max fall speed for the cars

const float TURN_SPEED = 50.0f;	// units per second
const float ROLL_SPEED = 80.0f;	// units per second
const float RESET_ROLL_SPEED = 30.0f;	// units per second
const float MAX_ROLL = 40.0f;	// amount the hover car can lean over in degrees

const float PITCH_SPEED = 10.0f;	// units per second
const float RESET_PITCH_SPEED = 10.0f;	// units per second
const float MAX_FOWARD_PITCH = 8.0f;	// the angle the car will tilt forward at when moving backwards
const float MAX_BACKWARD_PITCH = 12.0f;	// the angle the car will tilt back at when moving forward

const float START_GRID_X_SPACING = 20.0f;	// how far apart, left to right, the cars are on the start line
const float START_GRID_Z_SPACING = 30.0f;	// how far apart, front to back, the cars are on the start line

const float STARTING_HITPOINTS = 100.0f;
const float DAMAGE_MULTIPLIER = 3.0f;

const float ROTATION_VEL_MIN = 10.0f;
const float ROTATION_VEL_MAX = 150.0f;	// the max amount of rotation that can be applied at any time after a collision
const float ROTATION_MULTI = 30.0f;	// how fast the rotation will be
const float ROTATION_VEL_DECAY = 400.0f;	// how quickly the rotation will slow

const float COL_RES_DIVISION = 10.0f;	// how much to increment the reverse direction by after a collision 

const int ENGINE_PARTICLE_COUNT = 40;
const float ENGINE_PARTICLE_LIFETIME = 0.02f;
const float ENGINE_PARTICLE_SCALE = 0.15f;
const float ENGINE_PARTICLE_TIME_OFFSET = 0.002f;
const float ENGINE_PARTICLE_GRAVITY = 0.0f;
const int ENGINE_PARTICLE_MIN_VEL = 35;
const int ENGINE_PARTICLE_MAX_VEL = 45;
const int ENGINE_PARTICLE_MIN_SPRAY_ANGLE = 0;
const int ENGINE_PARTICLE_MAX_SPRAY_ANGLE = 10;
const int ENGINE_PARTICLE_MIN_CONE_ANGLE = -180;
const int ENGINE_PARTICLE_MAX_CONE_ANGLE = 180;
const int ENGINE_PARTICLE_Z_OFFSET = 80;	// is divided by 100
const int ENGINE_PARTICLE_STARTING_MAX_RADIUS = 50;	// is divided by 100
const float ENGINE_PARTICLE_EXHAUST_Y = 2.7f;
const float ENGINE_PARTICLE_EXHAUST_Z = -7.5f;
const float ENGINE_PARTICLE_EXHAUST_X_OFFSET = -0.5f;


//////////////
//	PLAYER	//
//////////////

const float FORWARD_MAX_SPEED = 1.42f;
const float BACKWARD_MAX_SPEED = -0.41f;

const float BOOST_HEAT_LIMIT = 3.0f;	// how long in seconds before the booster over-heats
const float BOOST_RECHARGE_SPEED = 4.0f;	// how many times slower than the useage
const float BOOST_RECHARGE_OVERHEAT = 8.0f;	// how many times slower than the useage
const float BOOST_MAX_PITCH = 18.0f;	// the angle the car will tilt back at when boosting forward
const float BOOST_MAX_SPEED = 2.02f;
const float BOOST_ACCELERATION = 1.6f;	// units per second per second
const float BOOSTER_TIMEOUT = 8.0f;	// how long after an over-heat you can use your booster again
const float DECELERATION_PERIOD = 3.0f;	// how long after an over-heat your engines will start working again

const int BOOST_PARTICLE_COUNT = 50;
const float BOOST_PARTICLE_LIFETIME = 0.03f;
const float BOOST_PARTICLE_SCALE = 0.1f;
const float BOOST_PARTICLE_TIME_OFFSET = 0.002f;
const float BOOST_PARTICLE_GRAVITY = 0.0f;
const int BOOST_PARTICLE_MIN_VEL = 40;
const int BOOST_PARTICLE_MAX_VEL = 50;
const int BOOST_PARTICLE_MIN_SPRAY_ANGLE = -5;
const int BOOST_PARTICLE_MAX_SPRAY_ANGLE = 5;
const int BOOST_PARTICLE_MIN_CONE_ANGLE = -180;
const int BOOST_PARTICLE_MAX_CONE_ANGLE = 180;
const int BOOST_PARTICLE_Z_OFFSET = 80;	// is divided by 100
const int BOOST_PARTICLE_STARTING_MIN_RADIUS = 50;	// is divided by 100
const int BOOST_PARTICLE_STARTING_MAX_RADIUS = 75;	// is divided by 100



//////////
//	AI	//
//////////

const float AI_MAX_SPEED = 1.4f;
const float AI_ACCEL = 1.0f;
const float AI_ACCEL_ANGLE_EIGHTH = 60.0f;	// (degrees) accelerate the AI car at an eighth speed if the angle to 
											// the next way point is below this but above AI_ACCEL_ANGLE_QUARTER
const float AI_ACCEL_ANGLE_QUARTER = 40.0f;	
const float AI_ACCEL_ANGLE_HALF = 20.0f;	// (degrees) accelerate the AI car at half speed if the angle to 
											// the next way point is below this but above AI_ACCEL_ANGLE_FULL
const float AI_ACCEL_ANGLE_FULL = 10.0f;// (degrees) accelerate the AI car at full speed as long as the 
										// angle to the next way point is below this
const float AI_TURN_DAMPING = 40.0f;	// the angle at which the AI will start to turn less towards its target
const float AI_WAYPOINT_RADIUS2 = CHECKPOINT_RADIUS2;	// distance from waypoint to activate next waypoint


//////////////
//	Bombs	//
//////////////

const int BOMB_PARTICLE_COUNT = 200;
const float BOMB_PARTICLE_LIFETIME = 0.3f;
const float BOMB_PARTICLE_SCALE = 0.01f;
const float BOMB_PARTICLE_GRAVITY = 5.0f;
const int BOMB_PARTICLE_MIN_VEL = 50;
const int BOMB_PARTICLE_MAX_VEL = 150;
const int BOMB_PARTICLE_MIN_SPRAY_ANGLE = 0;
const int BOMB_PARTICLE_MAX_SPRAY_ANGLE = 90;
const int BOMB_PARTICLE_MIN_CONE_ANGLE = -180;
const int BOMB_PARTICLE_MAX_CONE_ANGLE = 180;
const float BOMB_BLAST_RADIUS2 = 1500.0f;	// squared
const float BOMB_BLAST_STRENGTH = 10.0f;	// throw strength
const float BOMB_BLAST_DAMAGE = 6.0f;	// dmg multiplier


//////////////
//	Camera	//
//////////////

const float CAM_DEFAULT_ANGLE = 10.0f;
const float CAM_DEFAULT_HEIGHT = 10.0f;
const float CAM_DEFAULT_Z_OFFSET = -30.0f;

const float CAM_MOVE_SPEED = 50.0f;
const float CAM_ROTATE_SPEED = 0.02f;
const float CAM_ROTATE_SPEED_EDITOR = 100.0f;

const float CAM_COCKPIT_YPOS = 5.0f;
const float CAM_COCKPIT_ZPOS = 1.0f;

const float CAM_EDITOR_SIDE_WIDTH = 50.0f;	// far from the edge of the screen before the camera starts to rotate


//////////////
//	Editor	//
//////////////

const float TEMPLATE_START_DISTANCE = 40.0f;
const float MOVE_DISTANCE_SPEED = 50.0f;
const int UI_HIT_BOX = 50;	// pixels from centre in X and Y
const float TEMPLATE_ROTATION_SPEED = 20.0f;
const float SCALE_SPEED = 0.8f;	// per second

const int TRACK_NAME_INPUT_XPOS = 500;
const int TRACK_NAME_INPUT_YPOS = 400;

const int UI_FIRST_COLUMN_POS = 100;
const int UI_SECOND_COLUMN_POS = 200;
const int UI_THIRD_COLUMN_POS = 300;

const float TRACK_SAVED_MAX_TIME = 2.0f;


//////////////
//	Other	//
//////////////

const int MENU_TITLE_WIDTH = 1000;
const float MENU_TITLE_Y_POS = 100.0f;	// distance from the top of the screen

const float GAME_SPEED = 1.0f;	// 1 is normal speed

const float SKYBOX_YPOS = -960.0f;

const float RACE_START_COUNTDOWN_TIME = -3.0f;	// what the count down starts at to begin a race 
const float GO_SHOW_TIME = 2.0f;	// how long the "GO!" stays on the screen after the race starts
const float CROSS_EXPIRE_TIME = 5.0f;	// how long after creation a cross is removed from its check point
const float CROSS_SCALE = 0.3f;
const float CROSS_X_POSITION_OFFSET = 5.0f;
const std::string PRESS_TO_START = "Press space to start...";
const std::string GO = "Go!";
const std::string RACE_RESULTS = "Race Results";

const float PI = 3.142f;


//////////////////////
//	model classes	//
//////////////////////

class Sphere
{
private:
	Sphere()
	{
	}

public:
	IModel* model;
	float scalar;
	float radius2;

	Sphere(IModel* mod, float scal, float rad) :
		model(mod), scalar(scal), radius2(rad)
	{
	}

	Sphere(const Sphere& sourse)
	{
		model = sourse.model;
		scalar = sourse.scalar;
		radius2 = sourse.radius2;
	}
};

class Box
{
private:
	Box()
	{
	}

public:
	IModel* model;
	float scalar;
	float width;
	float length;

	Box(IModel* mod, float scal, float wid, float len) :
		model(mod), scalar(scal), width(wid), length(len)
	{
	}

	Box(const Box& sourse)
	{
		model = sourse.model;
		scalar = sourse.scalar;
		width = sourse.width;
		length = sourse.length;
	}
};

// for track builder UI models
class UIBox
{
public:
	string name;
	IMesh* mesh;
	IModel* model;
	int xPos;
	int yPos;
	int size;

	UIBox(string name = "", IMesh* mesh = nullptr, IModel* mod = nullptr, int xPos = 0, int yPos = 0, int width = 0) :
		name(name), mesh(mesh), model(mod), xPos(xPos), yPos(yPos), size(width)
	{
	}
};