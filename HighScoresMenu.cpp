// Neil Osbaldeston

#include <TL-Engine.h>
#include "dirent.h"
#include "Menu.h"
#include "Media.h"
#include "Globals.h"
#include "HighScoresMenu.h"

using namespace tle;

// constructor
HighScoresMenu::HighScoresMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode) : Menu(myEngine, mode)
{
	menuState = &state;
	backdrop = myEngine->CreateSprite(MENU_BACKDROP, 0.0f, 0.0f, 1.0f);

	createTextColBoxes();
}

// destructor
HighScoresMenu::~HighScoresMenu()
{
	delete backdrop;
	backdrop = nullptr;
}

// get the collision box for each menu option by looking at the size of the font used
void HighScoresMenu::createTextColBoxes()
{
	int height = textNormal->MeasureTextHeight(RACE);

	menuOptions.push_back(new MenuOption({ BACK, textNormal->MeasureTextWidth(BACK), height }));
}

// select a track and number of AI before starting a race
string HighScoresMenu::draw(GameStates& gameState)
{
	updateMousePos();
	return selectMenuOption(gameState, menuOptions, textNormal, textLarge, 200, 200, MENU_SPACING, kLeft);
}