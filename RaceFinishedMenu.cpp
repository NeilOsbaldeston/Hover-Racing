// Neil Osbaldeston

#include <TL-Engine.h>
#include "Media.h"
#include "Globals.h"
#include "RaceFinishedMenu.h"

using namespace tle;

// constructor
RaceFinishedMenu::RaceFinishedMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode) :
	Menu(myEngine, mode)
{
	menuState = &state;

	createTextColBoxes();
}

// destructor
RaceFinishedMenu::~RaceFinishedMenu()
{
}

// get the collision box for each menu option by looking at the size of the font used
void RaceFinishedMenu::createTextColBoxes()
{
	int height = textNormal->MeasureTextHeight(CONTINUE);

	menuOptions.push_back(new MenuOption({ RACE_AGAIN, textNormal->MeasureTextWidth(RACE_AGAIN), height }));
	menuOptions.push_back(new MenuOption({ BACK_TO_MENU, textNormal->MeasureTextWidth(BACK_TO_MENU), height }));
}

// select a track and number of AI before starting a race
string RaceFinishedMenu::draw(GameStates& gameState)
{
	updateMousePos();
	return selectMenuOption(gameState, menuOptions, textNormal, textLarge, halfScreenWidth, halfScreenHeight + 300, MENU_SPACING);
}