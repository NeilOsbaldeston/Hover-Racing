// Neil Osbaldeston

#include <TL-Engine.h>
#include <sstream>
#include "dirent.h"
#include "Menu.h"
#include "Media.h"
#include "Globals.h"
#include "RaceMenu.h"
#include "Extras.h"

using namespace tle;

// constructor
RaceMenu::RaceMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode) :
	Menu(myEngine, mode), carsAI(1), numLaps(3)
{
	menuState = &state;

	backdrop = myEngine->CreateSprite(MENU_BACKDROP, 0.0f, 0.0f, 1.0f);
	trackBox = myEngine->CreateSprite(TRACK_SELECTION_BOX, static_cast<float>(halfScreenWidth), 
		static_cast<float>(halfScreenHeight / 2));
	lapsBox = myEngine->CreateSprite(SELECTION_BOX, static_cast<float>(halfScreenWidth), 
		static_cast<float>(halfScreenHeight / 2 + TRACK_BOX_HEIGHT + MENU_BOX_SPACING));
	opponentsBox = myEngine->CreateSprite(SELECTION_BOX, static_cast<float>(halfScreenWidth + TRACK_BOX_WIDTH - SELECTION_BOX_WIDTH),
		static_cast<float>(halfScreenHeight / 2 + TRACK_BOX_HEIGHT + MENU_BOX_SPACING));

	listFont = myEngine->LoadFont(TRACKLIST_FONT, TRACKLIST_FONT_SIZE);

	createTextColBoxes();

	// prevent attempting to load a track with no data (returns to main menu if failed)
	if (!getTrackList() || tracks.size() == 0)
		throw;

	// initialise to the first track
	track = tracks[0];
}

// destructor
RaceMenu::~RaceMenu()
{
	delete backdrop;
	backdrop = nullptr;
	delete trackBox;
	trackBox = nullptr;
	delete lapsBox;
	lapsBox = nullptr;
	delete opponentsBox;
	opponentsBox = nullptr;
}

// get the collision box for each menu option by looking at the size of the font used
void RaceMenu::createTextColBoxes()
{
	int height = textNormal->MeasureTextHeight(RACE);

	menuOptions.push_back(new MenuOption({ START, textNormal->MeasureTextWidth(START), height }));
	menuOptions.push_back(new MenuOption({ BACK, textNormal->MeasureTextWidth(BACK), height }));
}

// allows the player to increase / decrease the number of laps by clicking on the up and down arrows attached to the selection box
void RaceMenu::changeLaps()
{
	// if the mouse is over the up arrow buttons
	if (myEngine->GetMouseX() > lapsBox->GetX() + SELECTION_BOX_WIDTH - SEL_BOX_BUTTON_WIDTH - widthAdjustment &&
		myEngine->GetMouseX() < lapsBox->GetX() + SELECTION_BOX_WIDTH - widthAdjustment &&
		myEngine->GetMouseY() > lapsBox->GetY() - heightAdjustment &&
		myEngine->GetMouseY() < lapsBox->GetY() + SEL_BOX_BUTTON_WIDTH - heightAdjustment)
	{
		if (myEngine->KeyHit(SELECT))
		{
			if (numLaps < 9)
				numLaps += 2;
		}
	}

	// if the mouse is over the down arrow buttons
	if (myEngine->GetMouseX() > lapsBox->GetX() + SELECTION_BOX_WIDTH - SEL_BOX_BUTTON_WIDTH - widthAdjustment &&
		myEngine->GetMouseX() < lapsBox->GetX() + SELECTION_BOX_WIDTH - widthAdjustment &&
		myEngine->GetMouseY() > lapsBox->GetY() + SELECTION_BOX_HEIGHT - SEL_BOX_BUTTON_WIDTH - heightAdjustment &&
		myEngine->GetMouseY() < lapsBox->GetY() + SELECTION_BOX_HEIGHT - heightAdjustment)
	{
		if (myEngine->KeyHit(SELECT))
		{
			if (numLaps > 1)
				numLaps -= 2;
		}
	}

	stringstream laps;

	if (numLaps == 1)
		laps << numLaps << " Lap";
	else
		laps << numLaps << " Laps";

	listFont->Draw(laps.str(), static_cast<int>(lapsBox->GetX()) + BOX_TEXT_X_OFFSET, 
		static_cast<int>(lapsBox->GetY()) + BOX_TEXT_Y_OFFSET, kBlack);
}

// allows the player to increase / decrease the number of AI opponents by clicking on the up and down arrows attached to the selection box
void RaceMenu::changeAICars()
{
	// if the mouse is over the up arrow buttons
	if (myEngine->GetMouseX() > opponentsBox->GetX() + SELECTION_BOX_WIDTH - SEL_BOX_BUTTON_WIDTH - widthAdjustment &&
		myEngine->GetMouseX() < opponentsBox->GetX() + SELECTION_BOX_WIDTH - widthAdjustment &&
		myEngine->GetMouseY() > opponentsBox->GetY() - heightAdjustment &&
		myEngine->GetMouseY() < opponentsBox->GetY() + SEL_BOX_BUTTON_WIDTH - heightAdjustment)
	{
		if (myEngine->KeyHit(SELECT))
		{
			if (carsAI < 7)
				++carsAI;
		}
	}

	// if the mouse is over the down arrow buttons
	if (myEngine->GetMouseX() > opponentsBox->GetX() + SELECTION_BOX_WIDTH - SEL_BOX_BUTTON_WIDTH - widthAdjustment &&
		myEngine->GetMouseX() < opponentsBox->GetX() + SELECTION_BOX_WIDTH - widthAdjustment &&
		myEngine->GetMouseY() > opponentsBox->GetY() + SELECTION_BOX_HEIGHT - SEL_BOX_BUTTON_WIDTH - heightAdjustment &&
		myEngine->GetMouseY() < opponentsBox->GetY() + SELECTION_BOX_HEIGHT - heightAdjustment)
	{
		if (myEngine->KeyHit(SELECT))
		{
			if (carsAI > 1)
				--carsAI;
		}
	}

	stringstream AICars;

	if (carsAI == 1)
		AICars << carsAI << " AI Car";
	else
		AICars << carsAI << " AI Cars";

	listFont->Draw(AICars.str(), static_cast<int>(opponentsBox->GetX()) + BOX_TEXT_X_OFFSET,
		static_cast<int>(opponentsBox->GetY()) + BOX_TEXT_Y_OFFSET, kBlack);
}

// select a track and number of AI before starting a race
string RaceMenu::draw(GameStates& gameState)
{
	updateMousePos();
	scrollTrackList();
	selectTrack();
	changeLaps();
	changeAICars();

	int horizontal = 0;
	int vertical = 0;

	if (FULLSCREEN)
		GetDesktopResolution(horizontal, vertical);
	else
	{
		horizontal = myEngine->GetWidth();
		vertical = myEngine->GetHeight();
	}

	return selectMenuOption(gameState, menuOptions, textNormal, textLarge, horizontal - 200, vertical - 200, MENU_SPACING);
}