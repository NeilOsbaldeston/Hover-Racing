// Neil Osbaldeston

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include "HoverRacing.h"
#include "Globals.h"
#include "Media.h"
#include "Race.h"
#include "TrackBuilder.h"
#include "MainMenu.h"
#include "RaceMenu.h"
#include "TrackBuilderMenu.h"
#include "HighScoresMenu.h"
#include "OptionsMenu.h"
#include "PauseMenu.h"
#include "RaceFinishedMenu.h"
#include "Extras.h"

using namespace tle;

// constructor
HoverRacing::HoverRacing() : 
	gameState(GameStates::MENU), menuState(MenuStates::MAIN_MENU), 
	mode(nullptr), editorOption("")
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	myEngine = New3DEngine(kTLX);

	GetDesktopResolution(horizontal, vertical);

	//myEngine->StartFullscreen(horizontal, vertical);
	myEngine->StartWindowed(WINDOWED_WIDTH, WINDOWED_HEIGHT);	// change FULLSCREEN const to false!!!!

	// Add folders for meshes and other media
	for (int i = 0; i < sizeof MEDIA_FOLDERS / sizeof MEDIA_FOLDERS[0]; ++i)
	{
		myEngine->AddMediaFolder(MEDIA_FOLDERS[i]);
	}

	// handle for menu system
	menu = new MainMenu(myEngine, menuState);

	raceState = new RaceStates{ RaceStates::NOT_STARTED };

	// initialise frame timer
	frameTimer = myEngine->Timer();
}

// destructor
HoverRacing::~HoverRacing()
{
	if (myEngine != nullptr)
	{
		// Delete the 3D engine now we are finished with it
		myEngine->Delete();
	}
}

// game loop
void HoverRacing::runGame()
{
	while (myEngine->IsRunning())
	{
		// update frame timer
		frameTimer = myEngine->Timer() * GAME_SPEED;

		// Draw the scene
		myEngine->DrawScene();

		// true when the window is in the foreground and selected
		if (myEngine->IsActive())
		{
			// pause control during a race or in the editor
			if (gameState != GameStates::MENU)
			{
				if (myEngine->KeyHit(PAUSE))
				{
					if (gameState == GameStates::PLAYING)
					{
						gameState = GameStates::PAUSED;

						if (menuState == MenuStates::EDITOR_SETUP)
							menu = new PauseMenu(myEngine, menuState, mode);
						else
							menu = new PauseMenu(myEngine, menuState);

						if (menuState == MenuStates::RACE_SETUP)
							myEngine->StopMouseCapture();
					}
					else
					{
						// clear the paused menu
						delete menu;
						menu = nullptr;

						gameState = GameStates::PLAYING;
						myEngine->GetMouseMovementX();
						myEngine->GetMouseMovementY();

						if (menuState == MenuStates::RACE_SETUP)
							myEngine->StartMouseCapture();
					}
				}
				// only check if we have the pause menu open
				else if (menu != nullptr)
				{
					// if the continue button was pressed we want to unpause the game, 
					// deleting the menu and resetting the flag
					if (menu->getContinuePressed())
					{
						menu->setContinuePressed(false);

						// clear the paused menu
						delete menu;
						menu = nullptr;

						gameState = GameStates::PLAYING;
						myEngine->GetMouseMovementX();
						myEngine->GetMouseMovementY();

						if (menuState == MenuStates::RACE_SETUP)
							myEngine->StartMouseCapture();
					}
				}
			}
			else // if in the menu
			{
				// menu stuff.....

				if (menuState == MenuStates::MAIN_MENU)
				{
					menu->draw(gameState);

					// delete the main menu
					if (menuState != MenuStates::MAIN_MENU)
					{
						delete menu;
						menu = nullptr;
					}

					// create sub-menu
					if (menuState == MenuStates::RACE_SETUP)
						menu = new RaceMenu(myEngine, menuState);
					else if (menuState == MenuStates::EDITOR_SETUP)
						menu = new TrackBuilderMenu(myEngine, menuState);
					else if (menuState == MenuStates::HIGH_SCORES)
						menu = new HighScoresMenu(myEngine, menuState);
					else if (menuState == MenuStates::OPTIONS)
						menu = new OptionsMenu(myEngine, menuState);

					// if sub-menu creation failed, go back to main menu
					if (menu == nullptr)
						menu = new MainMenu(myEngine, menuState);
				}
				// run chosen sub-menu
				else if (menuState == MenuStates::RACE_SETUP)
				{
					menu->draw(gameState);

					if (gameState == GameStates::PLAYING)
					{
						mode = new Race(myEngine, raceState, menu->getTrack(), menu->getCarsAI(), menu->getNumLaps());
						delete menu;
						menu = nullptr;
						myEngine->StartMouseCapture();
					}
					else if (menuState == MenuStates::MAIN_MENU)
					{
						// if back was selected
						delete menu;
						menu = new MainMenu(myEngine, menuState);
					}
				}
				else if (menuState == MenuStates::EDITOR_SETUP)
				{
					editorOption = menu->draw(gameState);

					if (gameState == GameStates::PLAYING)
					{
						if (editorOption == "")
							mode = new TrackBuilder(myEngine);
						else
							mode = new TrackBuilder(myEngine, menu->getTrack());

						delete menu;
						menu = nullptr;
					}
					else if (menuState == MenuStates::MAIN_MENU)
					{
						// if back was selected
						delete menu;
						menu = new MainMenu(myEngine, menuState);
					}
				}
				else if (menuState == MenuStates::HIGH_SCORES)
				{
					menu->draw(gameState);

					if (menuState == MenuStates::MAIN_MENU)
					{
						// if back was selected
						delete menu;
						menu = new MainMenu(myEngine, menuState);
					}
				}
				else if (menuState == MenuStates::OPTIONS)
				{
					menu->draw(gameState);

					if (menuState == MenuStates::MAIN_MENU)
					{
						// if back was selected
						delete menu;
						menu = new MainMenu(myEngine, menuState);
					}
				}
			}	// end of menu stuff

			if (gameState == GameStates::PLAYING)
			{
				// game stuff.....

				mode->start(frameTimer, &gameState, &menuState);

				if (*raceState == RaceStates::FINISHED && menuState == MenuStates::RACE_SETUP)
				{
					if (menu == nullptr)
						menu = new RaceFinishedMenu(myEngine, menuState);

					menu->draw(gameState);

					// exit if gameState changes to menu
					exitPlayingMode();

					if (menu->getRaceAgainPressed())
					{
						// get details of the current race
						string track = mode->getTrackName();
						int AI = mode->getNumCarsAI();
						int laps = mode->getLaps();

						// reset the race
						delete mode;
						mode = new Race(myEngine, raceState, track, AI, laps);

						// delete the finish race menu
						delete menu;
						menu = nullptr;
					}
				}
			}	// end of game stuff
			else if (gameState == GameStates::PAUSED)
			{
				if (myEngine->KeyHit(ESCAPE))
					menu->setContinuePressed(true);	// unpause

				// if paused
				menu->draw(gameState);	// pause menu

				// exit if gameState changes to menu
				exitPlayingMode();
			}
		}
		else  // if window inactive
		{
			if (gameState == GameStates::PLAYING)
			{
				gameState = GameStates::PAUSED;

				if (menuState == MenuStates::EDITOR_SETUP)
					menu = new PauseMenu(myEngine, menuState, mode);
				else
					menu = new PauseMenu(myEngine, menuState);

				if (menuState == MenuStates::RACE_SETUP)
					myEngine->StopMouseCapture();
			}

			myEngine->StopMouseCapture();
		}
	}
}

// exit if gameState changes to menu
void HoverRacing::exitPlayingMode()
{
	// if game mode exited
	if (gameState == GameStates::MENU)
	{
		delete mode;
		mode = nullptr;

		if (menu != nullptr)
		{
			delete menu;
			menu = nullptr;
		}

		if (menuState == MenuStates::MAIN_MENU)
			menu = new MainMenu(myEngine, menuState);
		else if (menuState == MenuStates::RACE_SETUP)
			menu = new RaceMenu(myEngine, menuState);
		else if (menuState == MenuStates::EDITOR_SETUP)
			menu = new TrackBuilderMenu(myEngine, menuState);
	}
}

// release resources
void HoverRacing::cleanUp()
{
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
	myEngine = nullptr;
}