// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>	// for onscreen text
#include <iomanip>
#include "Globals.h"
#include "Media.h"
#include "Race.h"
#include "RaceCourse.h"
#include "Player_HoverCar.h"
#include "AI_HoverCar.h"
#include "MyCamera.h"

using namespace tle;


// constructor
Race::Race(I3DEngine* myEngine, RaceStates*& state, string trackName, int numAI, int numLaps) :
	GameMode(myEngine), myEngine(myEngine), raceState(state), startCountDown(false),
	raceStartTimer(RACE_START_COUNTDOWN_TIME), showDebugInfo(false), currentBoost(0), 
	playerPos(0), racePosBorder(nullptr)
{
	*raceState = RaceStates::NOT_STARTED;
	track = trackName;
	maxLaps = numLaps;
	numCarsAI = numAI;

	// create sprites
	backdropUI = myEngine->CreateSprite(UI_BACKDROP, 0.0f, static_cast<float>(myEngine->GetHeight()) - UI_HEIGHT);
	boostmeter = myEngine->CreateSprite(BOOSTMETER_EMPTY, static_cast<float>(myEngine->GetWidth() - 
		BOOSTMETER_WIDTH), static_cast<float>(myEngine->GetHeight() - BOOSTMETER_HEIGHT));

	// create fonts
	debugFont = myEngine->LoadFont(DEBUG_FONT, DEBUG_FONT_SIZE);
	countdownFont = myEngine->LoadFont(COUNTDOWN_FONT, COUNTDOWN_FONT_SIZE);
	uiFont = myEngine->LoadFont(UI_FONT, UI_FONT_SIZE);

	// create the race course
	raceCourse.createTrack(trackName);	// make dynamic
	raceCourse.partitionWorld();
	raceCourse.createBombParticles();

	// create the camera
	dummyMesh = myEngine->LoadMesh(DUMMY);

	// load the cross mesh for check point completion
	crossMesh = myEngine->LoadMesh(CROSS);

	// create hover car
	if (raceCourse.getStartPoint() != nullptr)
	{
		// extract the start point data
		float posMatrix[4][4];
		raceCourse.getStartPoint()->GetMatrix(&posMatrix[0][0]);

		// create the players car
		hoverCarPlayer = new Player_HoverCar(myEngine, RACE_2_RED, &raceCourse);
		hoverCarPlayer->setDriverName("Player");

		// rotate the players car to match the starting orientation
		hoverCarPlayer->getModel()->SetMatrix(&posMatrix[0][0]);

		// if the number of cars on the grid is even, we swap the X positions around to make sure pole position is always on the left
		if (numAI % 2)
			hoverCarPlayer->getModel()->MoveLocalX(START_GRID_X_SPACING);
		
		// position the player car at the back of the grid
		hoverCarPlayer->getModel()->MoveLocalZ(-START_GRID_Z_SPACING * numAI);

		// get the player car matrix
		float playerMatrix[4][4];
		hoverCarPlayer->getModel()->GetMatrix(&playerMatrix[0][0]);
		
		// create the dummy and align to match the car
		if (dummyMesh)
		dummy = dummyMesh->CreateModel();
		dummy->SetMatrix(&playerMatrix[0][0]);
	}
	else  // create a player car and dummy at the default location
	{
		hoverCarPlayer = new Player_HoverCar(myEngine, RACE_2, &raceCourse);

		// position the player car at the back of the grid
		hoverCarPlayer->getModel()->MoveLocalZ(-START_GRID_Z_SPACING * numAI);

		// if the number of cars on the grid is even, we swap the X positions around to make sure pole position is always on the left
		if (numAI % 2)
			hoverCarPlayer->getModel()->MoveLocalX(START_GRID_X_SPACING);

		if (dummyMesh)
			dummy = dummyMesh->CreateModel(DEFAULT_START_XPOS, DEFAULT_START_YPOS, DEFAULT_START_ZPOS + (-START_GRID_Z_SPACING * numAI));
	}
	
	// get the player car matrix
	float playerMatrix[4][4];
	hoverCarPlayer->getModel()->GetMatrix(&playerMatrix[0][0]);

	// set up the AI cars
	bool firstAI = true;
	int countAI = 1;
	for (int rows = 0; rows < 4; ++rows)
	{
		for (int cols = 0; cols < 2; ++cols)
		{
			// account for the players car
			if (firstAI)
			{
				++cols;
				firstAI = false;
			}

			// position the AI cars in a grid formation
			if (countAI <= numAI)
			{
				AI_HoverCar* carAI = new AI_HoverCar(myEngine, RACE_2, &raceCourse);

				// set the AI name
				stringstream name;
				name << "AI_" << countAI;
				carAI->setDriverName(name.str());

				carAI->getModel()->SetMatrix(&playerMatrix[0][0]);

				// if the num car on the grid is even, we swap the X positions around to make sure pole position is always on the left
				if (numAI % 2)
					carAI->getModel()->MoveLocalX(-START_GRID_X_SPACING * cols);
				else
					carAI->getModel()->MoveLocalX(START_GRID_X_SPACING * cols);

				carAI->getModel()->MoveLocalZ(START_GRID_Z_SPACING * countAI);

				carAI->getModel()->RotateLocalX(START_ANGLE_OFFSET);

				// setup the engine particles for the AI cars
				float timeOffset = 0.0f;
				for (Particle* particle : carAI->getEngineParticles())
				{
					carAI->setEngineParticlePositions(particle, carAI->getModel());
					particle->increaseLifeTimer(timeOffset);

					timeOffset += ENGINE_PARTICLE_TIME_OFFSET;
				}

				// add AI car to the array
				carsAI.push_back(carAI);

				++countAI;
			}
		}
	}

	// rotate the car to nicer angle (done after setting up AI so setting their local Z isnt messed up)
	hoverCarPlayer->getModel()->RotateLocalX(START_ANGLE_OFFSET);

	// setup the engine particles for the players car
	float timeOffset = 0.0f;
	for (Particle* particle : hoverCarPlayer->getEngineParticles())
	{
		hoverCarPlayer->setEngineParticlePositions(particle, hoverCarPlayer->getModel());
		particle->increaseLifeTimer(timeOffset);

		timeOffset += ENGINE_PARTICLE_TIME_OFFSET;
	}
	timeOffset = 0.0f;
	for (Particle* particle : hoverCarPlayer->getBoostParticles())
	{
		hoverCarPlayer->setBoostParticlePositions(particle, hoverCarPlayer->getModel());
		particle->increaseLifeTimer(timeOffset);

		timeOffset += BOOST_PARTICLE_TIME_OFFSET;
	}

	// provide the cars with a list of all cars to use for collision tests
	hoverCarPlayer->setCarLists(hoverCarPlayer, carsAI);
	for (int countAI = 0; countAI < numAI; ++countAI)
	{
		carsAI[countAI]->setCarLists(hoverCarPlayer, carsAI);
	}

	myCamera.setParent(dummy);
}

// destructor
Race::~Race()
{
	delete hoverCarPlayer;
	hoverCarPlayer = nullptr;
	for (int i = 0; i < static_cast<int>(carsAI.size()); ++i)
	{
		delete carsAI[i];
		carsAI[i] = nullptr;
	}

	for (auto it = crosses.begin(); it != crosses.end(); ++it)
	{
		crossMesh->RemoveModel((*it).first);
	}

	if (backdropUI != nullptr)
		delete backdropUI;

	if (racePosBorder != nullptr)
		delete racePosBorder;

	delete boostmeter;
}

// counts down for 3 seconds in iterations of a second then releases control back to the parent function
bool Race::raceStartCountDown(float frameTimer)
{
	raceStartTimer += frameTimer;
	int nTime = static_cast<int>(fabsf(raceStartTimer - 1.0f));	// + 1 second to show first number for a second
	stringstream time;
	time << nTime;

	countdownFont->Draw(time.str(), myEngine->GetWidth() / 2 + 2, myEngine->GetHeight() / 3 + 2, kWhite, kCentre);
	countdownFont->Draw(time.str(), myEngine->GetWidth() / 2, myEngine->GetHeight() / 3, kBlack, kCentre);

	if (raceStartTimer >= 0.0f)
	{
		raceStartTimer = 0.0f;
		return true;
	}
	
	return false;
}

// put all cars in a single array and do stage and lap checks on each
bool Race::checkRacePositions()
{
	vector<HoverCar*> cars;	// generic car array
	vector<HoverCar*> carsPos;	// generic car array
	racePositions.clear();

	// add all cars (player and AI) to one array
	cars.push_back(dynamic_cast<HoverCar*>(hoverCarPlayer));

	for (int car = 0; car < static_cast<int>(carsAI.size()); ++car)
	{
		cars.push_back(dynamic_cast<HoverCar*>(carsAI[car]));
	}

	// work out race positions
	for (int lap = maxLaps; lap > 0; --lap)
	{		
		// check all cars on the same lap to see what stage they are on
		for (int stage = raceCourse.getCheckPoints().size(); stage > 0; --stage)
		{
			// empty the array so we only hold the cars on the same stage of the same lap
			carsPos.clear();

			// check for cars on the checked lap
			for (auto car = cars.begin(); car != cars.end(); ++car)
			{
				if ((*car)->getCurrentLap() == lap)
				{
					if ((*car)->getCurrentStage() == stage)
					{
						carsPos.push_back((*car));
					}
				}
			}

			// providing theres a car in that stage
			if (carsPos.size() > 0)
			{
				// if only 1 car, no need to sort
				if (carsPos.size() > 1)
				{
					// iterate through the carsPos array and use insertion sort to arrange 
					// the array by how close to the next check point they are
					for (unsigned int index = 1; index < carsPos.size(); ++index)
					{
						HoverCar* insert = carsPos[index];
						int j = index;

						while (j > 0 && (carsPos[j - 1]->getCheckPointDistance2() > insert->getCheckPointDistance2()))
						{
							carsPos[j] = carsPos[j - 1];

							--j;
						}
						carsPos[j] = insert;
					}
					// put the cars into the racePositions array
					for (auto car = carsPos.begin(); car != carsPos.end(); ++car)
					{
						racePositions.push_back(*car);
					}
				}
				else
					racePositions.push_back(carsPos.front());
			}
		}
	}	// race position end

	// set player position
	int position = 1;
	for (auto pos = racePositions.begin(); pos != racePositions.end(); ++pos)
	{
		if ((*pos)->getModel() == hoverCarPlayer->getModel())
		{
			playerPos = position;
		}
		++position;
	}

	// check to see if a check point has been passed
	for (auto car = cars.begin(); car != cars.end(); ++car)
	{
		if (!(*car)->getFinished())
		{
			// returns true if the player passes the finish line
			if (stageCheck(*car))
				return true;
		}
	}

	return false;
}

// check to see if the player passes within range of the next check point
bool Race::stageCheck(HoverCar* car)
{
	// if stage is at x, check player collision with x - 1
	Sphere* currentCheckpoint = raceCourse.getCheckPoints()[car->getCurrentStage() - 1];

	// get the player car location
	float carX = car->getModel()->GetX();
	float carZ = car->getModel()->GetZ();

	// get the check point location
	float checkPointX = currentCheckpoint->model->GetX();
	float checkPointZ = currentCheckpoint->model->GetZ();
	
	// get the vector between the players car and the check point
	float vecX = checkPointX - carX;
	float vecZ = checkPointZ - carZ;

	// get the distance between the players car and the check point
	car->setCheckPointDistance2(vecX * vecX + vecZ * vecZ);

	float scale = currentCheckpoint->scalar;

	// find out if theres a collision
	if (car->getCheckPointDistance2() < CHECKPOINT_RADIUS2 * (scale * scale))
	{
		float matrix[4][4];
		currentCheckpoint->model->GetMatrix(&matrix[0][0]);

		// only place crosses for the player
		if (car->getModel() == hoverCarPlayer->getModel())
		{
			// create a cross at the checkpoint to indicate a successful pass
			IModel* cross = crossMesh->CreateModel();
			cross->SetMatrix(&matrix[0][0]);
			cross->Scale(CROSS_SCALE);
			cross->MoveY(CROSS_X_POSITION_OFFSET * scale);
			crosses.push_back(make_pair(cross, 0.0f));
		}

		// provided theres more check points, move to the next else finish the race
		if (car->getCurrentStage() < raceCourse.getStageCount())
			car->setCurrentStage(car->getCurrentStage() + 1);
		else if (car->getCurrentLap() < maxLaps)
		{
			car->setCurrentLap(car->getCurrentLap() + 1);
			car->setCurrentStage(1);
		}
		else if (car->getModel() == hoverCarPlayer->getModel())
		{
			finishPositions.push_back(make_pair(car, raceStartTimer));
			car->setFinished(true);
			return true; // only end the race when the player crosses the finish line
		}
		else
		{
			finishPositions.push_back(make_pair(car, raceStartTimer));
			car->setFinished(true);
		}
	}

	return false;
}

// prints the frame rate info in the top corner of the screen
void Race::outputDebugInfo(float frameTimer)
{
	stringstream outText;
	outText << "Frame time: " << fixed << setprecision(4) << frameTimer;
	debugFont->Draw(outText.str(), 15, 15, kYellow);
	outText.str(""); // Clear myStream
	outText << "FPS: " << fixed << setprecision(2) << 1.0f / frameTimer;
	debugFont->Draw(outText.str(), 15, 25, kYellow);
	outText.str(""); // Clear myStream
}

// print race info like speed, lap, stage, hitpoints
void Race::printRaceInfo()
{
	stringstream outText;

	if (hoverCarPlayer->getHitPoints() > 0)
	{

		if (*raceState == RaceStates::FINISHED)
		{
			// display race data (place, score, time etc)
			countdownFont->Draw(RACE_RESULTS, myEngine->GetWidth() / 2, myEngine->GetHeight() / 10, kBlack, kCentre);

			string positionPrefix = "";
			int index = 1;

			outText << "Place";
			uiFont->Draw(outText.str(), myEngine->GetWidth() / 2 - 250, myEngine->GetHeight() / 4 - 30, kBlack);
			outText.str("");
			outText << "Name";
			uiFont->Draw(outText.str(), myEngine->GetWidth() / 2 - 150, myEngine->GetHeight() / 4 - 30, kBlack);
			outText.str("");
			outText << "Race Time";
			uiFont->Draw(outText.str(), myEngine->GetWidth() / 2 + 100, myEngine->GetHeight() / 4 - 30, kBlack);
			outText.str("");

			// as the cars cross the finish line (after the player) the list will grow on the screen showing placing and time
			for (auto car = finishPositions.begin(); car != finishPositions.end(); ++car)
			{
				positionPrefix = "";

				if (index == 1)
					positionPrefix = "st";
				else if (index == 2)
					positionPrefix = "nd";
				else if (index == 3)
					positionPrefix = "rd";
				else
					positionPrefix = "th";

				outText << index << positionPrefix;
				uiFont->Draw(outText.str(), myEngine->GetWidth() / 2 - 250, myEngine->GetHeight() / 4 + (50 * index), kBlack);
				outText.str("");
				outText << car->first->getDriverName();
				uiFont->Draw(outText.str(), myEngine->GetWidth() / 2 - 150, myEngine->GetHeight() / 4 + (50 * index), kBlack);
				outText.str("");
				outText << formatRaceTime(car->second);
				uiFont->Draw(outText.str(), myEngine->GetWidth() / 2 + 100, myEngine->GetHeight() / 4 + (50 * index), kBlack);
				outText.str("");

				++index;
			}
		}
		else
		{
			outText << "Race time: " << formatRaceTime(raceStartTimer);
			uiFont->Draw(outText.str(), 10, myEngine->GetHeight() - UI_FONT_SIZE, kBlack);
			outText.str("");
			outText << "Position: " << playerPos;
			uiFont->Draw(outText.str(), 330, myEngine->GetHeight() - UI_FONT_SIZE, kBlack);
			outText.str("");
			outText << "Hit points " << hoverCarPlayer->getHitPoints();
			uiFont->Draw(outText.str(), 500, myEngine->GetHeight() - UI_FONT_SIZE, kBlack);
			outText.str("");
		}

		outText << static_cast<int>(hoverCarPlayer->getSpeed() * 50.0f);
		countdownFont->Draw(outText.str(), myEngine->GetWidth() - BOOSTMETER_WIDTH / 2, myEngine->GetHeight() - (BOOSTMETER_HEIGHT / 4) * 3, kGreen, kCentre);
		outText.str("");
		outText << "Lap " << hoverCarPlayer->getCurrentLap() << "/" << maxLaps;
		uiFont->Draw(outText.str(), myEngine->GetWidth() - BOOSTMETER_WIDTH / 2, myEngine->GetHeight() - UI_FONT_SIZE * 2, kGreen, kCentre);
		outText.str("");
		outText << "Stage " << hoverCarPlayer->getCurrentStage();
		uiFont->Draw(outText.str(), myEngine->GetWidth() - BOOSTMETER_WIDTH / 2, myEngine->GetHeight() - UI_FONT_SIZE, kGreen, kCentre);
		outText.str("");
	}
	else
	{
		uiFont->Draw("Your car has brokedown, you were forced to retire from the race", myEngine->GetWidth() / 2, myEngine->GetHeight() / 10, kBlack, kCentre);
	}
}

// turn the floating point race time into a HH:MM:SS.00 format
string Race::formatRaceTime(float time)
{
	stringstream outText;

	int hours = static_cast<int>(time) / 3600;
	int minutes = static_cast<int>(time) / 60 - (hours * 3600);
	float seconds = time - (60 * minutes);

	outText << hours << ":" << setfill('0') << setw(2) << minutes << 
		":" << setfill('0') << setw(5) << fixed << setprecision(2) << seconds;
	
	return outText.str();
}

// display the boost meter with the correct amount of bars
void Race::displayBoostMeter()
{
	if (hoverCarPlayer->getBoostHeat() <= 0.001f && currentBoost != 0)
		boostImage(BOOSTMETER_EMPTY, 0);
	else if (hoverCarPlayer->getBoostHeat() > BOOST_HEAT_LIMIT / 15.0f && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 2 && currentBoost != 1)
		boostImage(BOOSTMETER_1, 1);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 2 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 3 && currentBoost != 2)
		boostImage(BOOSTMETER_2, 2);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 3 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 4 && currentBoost != 3)
		boostImage(BOOSTMETER_3, 3);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 4 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 5 && currentBoost != 4)
		boostImage(BOOSTMETER_4, 4);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 5 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 6 && currentBoost != 5)
		boostImage(BOOSTMETER_5, 5);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 6 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 7 && currentBoost != 6)
		boostImage(BOOSTMETER_6, 6);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 7 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 8 && currentBoost != 7)
		boostImage(BOOSTMETER_7, 7);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 8 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 9 && currentBoost != 8)
		boostImage(BOOSTMETER_8, 8);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 9 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 10 && currentBoost != 9)
		boostImage(BOOSTMETER_9, 9);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 10 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 11 && currentBoost != 10)
		boostImage(BOOSTMETER_10, 10);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 11 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 12 && currentBoost != 11)
		boostImage(BOOSTMETER_11, 11);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 12 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 13 && currentBoost != 12)
		boostImage(BOOSTMETER_12, 12);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 13 && hoverCarPlayer->getBoostHeat() < (BOOST_HEAT_LIMIT / 15.0f) * 14 && currentBoost != 13)
		boostImage(BOOSTMETER_13, 13);
	else if (hoverCarPlayer->getBoostHeat() > (BOOST_HEAT_LIMIT / 15.0f) * 14 && hoverCarPlayer->getBoostHeat() < BOOST_HEAT_LIMIT && currentBoost != 14)
		boostImage(BOOSTMETER_14, 14);
}

// delete the old image and create a new one
void Race::boostImage(const string& sprite, int imageNum)
{
	currentBoost = imageNum;
	delete boostmeter;
	boostmeter = myEngine->CreateSprite(sprite, static_cast<float>(myEngine->GetWidth() - 
		BOOSTMETER_WIDTH), static_cast<float>(myEngine->GetHeight() - BOOSTMETER_HEIGHT), 0.1f);
}

// remove crosses after a time period
void Race::decayCrosses(float frameTimer)
{
	int deleteCrosses = 0;
	for (auto it = crosses.begin(); it != crosses.end(); ++it)
	{
		if ((*it).second > CROSS_EXPIRE_TIME)
		{
			crossMesh->RemoveModel((*it).first);
			++deleteCrosses;
		}
		else
			(*it).second += frameTimer;
	}

	for (int i = 0; i < deleteCrosses; ++i)
	{
		crosses.pop_front();
	}
}

void Race::explodeBombs(float frameTimer)
{
	// go through the bombs - any that are active, animate its particles
	vector<Bomb*> tempBombs;
	for (Bomb* bomb : raceCourse.getBombs())
	{
		if (bomb->getActivated())
		{
			// create a variable that will only be set to true if particles are still in play
			bool stillActive = false;
			// go through each particle
			for (Particle* particle : bomb->getParticles())
			{
				if (!particle->getExpired())
				{
					stillActive = true;
					particle->manipulateParticle(frameTimer);

					particle->getModel()->LookAt(myCamera.getModel());

					if (particle->getLifeTimer() > BOMB_PARTICLE_LIFETIME)
					{
						particle->setExpired(true);
						bomb->expireExplosionParticles(particle, bomb->getModel());
					}
				}
			}
			if (!stillActive)
			{
				bomb->getModel()->MoveX(-10000.0f);
				tempBombs.push_back(bomb);
			}
		}
	}
	// no longer search through the expired bombs
	for (Bomb* temp : tempBombs)
	{
		raceCourse.getBombs().remove(temp);
	}
}

// control the race from start to finish including timers, ui, stages/check points etc.
void Race::start(float frameTimer, GameStates* gameState, MenuStates* menuState)
{
	if (*raceState == RaceStates::NOT_STARTED)
	{
		printRaceInfo();
		// press space to count down to start...
		if (myEngine->KeyHit(START_RACE))
			startCountDown = true;

		if (!startCountDown)
		{
			countdownFont->Draw(PRESS_TO_START, myEngine->GetWidth() / 2 + 2, myEngine->GetHeight() / 3 + 2, kWhite, kCentre);
			countdownFont->Draw(PRESS_TO_START, myEngine->GetWidth() / 2, myEngine->GetHeight() / 3, kBlack, kCentre);
		}

		if (startCountDown)
		{
			if (raceStartCountDown(frameTimer))
			{
				myEngine->GetMouseMovementX();	// call to remove any unwanted movement
				myEngine->GetMouseMovementY();	// call to remove any unwanted movement
				*raceState = RaceStates::IN_PROGRESS;
			}
		}
	}
	else if (*raceState == RaceStates::IN_PROGRESS)
	{
		raceStartTimer += frameTimer;	// also can be used to track the time elapsed for a race

		if (raceStartTimer <= GO_SHOW_TIME)
		{
			countdownFont->Draw(GO, myEngine->GetWidth() / 2 + 2, myEngine->GetHeight() / 3 + 2, kWhite, kCentre);
			countdownFont->Draw(GO, myEngine->GetWidth() / 2, myEngine->GetHeight() / 3, kBlack, kCentre);
		}
			

		// calculate car movement, move the car and the dummy
		hoverCarPlayer->moveHoverCar(debugFont, dummy, frameTimer, showDebugInfo);

		// move the AI controlled cars
		for (auto carAI = carsAI.begin(); carAI != carsAI.end(); ++carAI)
		{
			(*carAI)->moveHoverCar(frameTimer);
		}

		// take care of collisions with bombs
		explodeBombs(frameTimer);

		// delete old crosses from check points
		decayCrosses(frameTimer);

		// update boost meter
		displayBoostMeter();

		// update the camera
		myCamera.moveCamera(frameTimer);
		myCamera.resetCamPos(hoverCarPlayer, dummy);

		if (hoverCarPlayer->getHitPoints() <= 0)
		{
			// when the player finishes the race
			delete backdropUI;
			backdropUI = nullptr;

			*raceState = RaceStates::FINISHED;
		}
		else if (checkRacePositions())
		{
			// when the player finishes the race
			delete backdropUI;
			backdropUI = nullptr;
			
			racePosBorder = myEngine->CreateSprite(RACE_POS_BORDER, static_cast<float>(myEngine->GetWidth() / 2 - RACE_POS_BORDER_WIDTH / 2), 
				static_cast<float>(myEngine->GetHeight() / 2 - RACE_POS_BORDER_HEIGHT / 2), 0.5f);

			*raceState = RaceStates::FINISHED;
		}
		else
			printRaceInfo();
	}
	else if (*raceState == RaceStates::FINISHED)
	{
		raceStartTimer += frameTimer;	// also can be used to track the time elapsed for a race

		// drift past the finish line till you come to a stop (could let the AI take over and do a victory lap)
		hoverCarPlayer->setFrameTime(frameTimer);
		hoverCarPlayer->resetRoll();
		hoverCarPlayer->resetPitch();
		hoverCarPlayer->move();
		hoverCarPlayer->coolDown();

		// delete old crosses from check points
		decayCrosses(frameTimer);

		// take care of collisions with bombs
		explodeBombs(frameTimer);

		// update readouts
		checkRacePositions();
		printRaceInfo();
		displayBoostMeter();

		// move the AI controlled cars
		for (auto carAI = carsAI.begin(); carAI != carsAI.end(); ++carAI)
		{
			(*carAI)->moveHoverCar(frameTimer);
		}
	}

	// do always //

	if (myEngine->KeyHit(TOGGLE_DEBUG_INFO))
		showDebugInfo = showDebugInfo ? showDebugInfo = false : showDebugInfo = true;

	if (showDebugInfo)
		outputDebugInfo(frameTimer);

	// move the player cars exhaust particles
	for (Particle* particle : hoverCarPlayer->getEngineParticles())
	{
		particle->manipulateParticle(frameTimer);
		hoverCarPlayer->expireEngineParticles(particle, hoverCarPlayer->getModel());
		particle->getModel()->LookAt(myCamera.getModel());
		particle->getModel()->Scale(ENGINE_PARTICLE_SCALE);
	}

	// control boost particles
	if (hoverCarPlayer->getBoostHeat() < BOOST_HEAT_LIMIT && myEngine->KeyHeld(BOOST) && hoverCarPlayer->getBoosterActive())
	{
		// if active and not overheating
		for (Particle* particle : hoverCarPlayer->getBoostParticles())
		{
			particle->manipulateParticle(frameTimer);
			hoverCarPlayer->expireBoostParticles(particle, hoverCarPlayer->getModel());
			particle->getModel()->LookAt(myCamera.getModel());
			particle->getModel()->Scale(BOOST_PARTICLE_SCALE);
		}
	}
	else
	{
		for (Particle* particle : hoverCarPlayer->getBoostParticles())
		{
			particle->getModel()->Move(
			hoverCarPlayer->getModel()->GetX(),
			hoverCarPlayer->getModel()->GetY() + 2.7f,
			hoverCarPlayer->getModel()->GetZ());
			particle->getModel()->LookAt(myCamera.getModel());
			particle->getModel()->Scale(0.001f);
		}
	}

	// move the AI's cars exhaust particles
	for (auto carAI = carsAI.begin(); carAI != carsAI.end(); ++carAI)
	{
		for (Particle* particle : (*carAI)->getEngineParticles())
		{
			particle->manipulateParticle(frameTimer);
			(*carAI)->expireEngineParticles(particle, (*carAI)->getModel());
			particle->getModel()->LookAt(myCamera.getModel());
			particle->getModel()->Scale(ENGINE_PARTICLE_SCALE);
		}
	}
}