// Neil Osbaldeston

#include <TL-Engine.h>
#include "dirent.h"
#include "Menu.h"
#include "Media.h"
#include "Globals.h"
#include "TrackBuilderMenu.h"
#include "Extras.h"

using namespace tle;

// constructor
TrackBuilderMenu::TrackBuilderMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode) : Menu(myEngine, mode)
{
	menuState = &state;
	backdrop = myEngine->CreateSprite(MENU_BACKDROP, 0.0f, 0.0f, 1.0f);
	trackBox = myEngine->CreateSprite(TRACK_SELECTION_BOX, static_cast<float>(halfScreenWidth), static_cast<float>(halfScreenHeight - 100));
	listFont = myEngine->LoadFont(TRACKLIST_FONT, TRACKLIST_FONT_SIZE);

	createTextColBoxes();

	// prevent attempting to load a track with no data (returns to main menu if failed)
	if (!getTrackList() || tracks.size() == 0)
		throw;

	// initialise to the first track
	track = tracks[0];
}

// destructor
TrackBuilderMenu::~TrackBuilderMenu()
{
	delete backdrop;
	backdrop = nullptr;
	delete trackBox;
	trackBox = nullptr;
}

// get the collision box for each menu option by looking at the size of the font used
void TrackBuilderMenu::createTextColBoxes()
{
	int height = textNormal->MeasureTextHeight(RACE);

	menuOptions.push_back(new MenuOption({ NEW, textNormal->MeasureTextWidth(NEW), height }));
	menuOptions.push_back(new MenuOption({ EDIT, textNormal->MeasureTextWidth(EDIT), height }));
	menuOptions.push_back(new MenuOption({ BACK, textNormal->MeasureTextWidth(BACK), height }));
}

// select a track and number of AI before starting a race
string TrackBuilderMenu::draw(GameStates& gameState)
{
	updateMousePos();
	scrollTrackList();
	selectTrack();

	int horizontal = 0;
	int vertical = 0;

	if (FULLSCREEN)
		GetDesktopResolution(horizontal, vertical);
	else
	{
		horizontal = myEngine->GetWidth();
		vertical = myEngine->GetHeight();
	}

	return selectMenuOption(gameState, menuOptions, textNormal, textLarge, horizontal / 2 - 150, vertical / 2 - 100, MENU_SPACING);
}