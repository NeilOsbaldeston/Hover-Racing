// Neil Osbaldeston

#include <TL-Engine.h>
#include "dirent.h"
#include "Menu.h"
#include "Media.h"
#include "Globals.h"
#include "MainMenu.h"

using namespace tle;

// constructor
MainMenu::MainMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode) : Menu(myEngine, mode)
{
	menuState = &state;
	title = myEngine->CreateSprite(MENU_TITLE, static_cast<float>(halfScreenWidth - MENU_TITLE_WIDTH / 2), MENU_TITLE_Y_POS);
	backdrop = myEngine->CreateSprite(MENU_BACKDROP);

	createTextColBoxes();
}

// destructor
MainMenu::~MainMenu()
{
	delete backdrop;
	backdrop = nullptr;

	delete title;
	title = nullptr;
}

// get the collision box for each menu option by looking at the size of the font used
void MainMenu::createTextColBoxes()
{
	int height = textNormal->MeasureTextHeight(RACE);

	menuOptions.push_back(new MenuOption({ RACE, textNormal->MeasureTextWidth(RACE), height }));
	menuOptions.push_back(new MenuOption({ TIME_TRIAL, textNormal->MeasureTextWidth(TIME_TRIAL), height }));
	menuOptions.push_back(new MenuOption({ TRACK_BUILDER, textNormal->MeasureTextWidth(TRACK_BUILDER) , height }));
	menuOptions.push_back(new MenuOption({ HIGH_SCORES, textNormal->MeasureTextWidth(HIGH_SCORES), height }));
	menuOptions.push_back(new MenuOption({ OPTIONS, textNormal->MeasureTextWidth(OPTIONS), height }));
	menuOptions.push_back(new MenuOption({ QUIT, textNormal->MeasureTextWidth(QUIT), height }));
}

// called each frame to keep the menu drawn on the screen
string MainMenu::draw(GameStates& gameState)
{
	updateMousePos();
	return selectMenuOption(gameState, menuOptions, textNormal, textLarge, halfScreenWidth, halfScreenHeight, MENU_SPACING);
}