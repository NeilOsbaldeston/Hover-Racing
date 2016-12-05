// Neil Osbaldeston

#include <TL-Engine.h>
#include "Media.h"
#include "Globals.h"
#include "PauseMenu.h"

using namespace tle;

// constructor
PauseMenu::PauseMenu(I3DEngine* myEngine, MenuStates& state, GameMode* mode) :
	Menu(myEngine, mode)
{
	menuState = &state;

	pauseOverlay = myEngine->CreateSprite(PAUSE_OVERLAY, static_cast<float>(halfScreenWidth - PAUSE_OVERLAY_WIDTH / 2),
		static_cast<float>(halfScreenHeight - PAUSE_OVERLAY_HEIGHT / 2));

	createTextColBoxes();
}

// destructor
PauseMenu::~PauseMenu()
{
	delete pauseOverlay;
	pauseOverlay = nullptr;
}

// get the collision box for each menu option by looking at the size of the font used
void PauseMenu::createTextColBoxes()
{
	int height = textNormal->MeasureTextHeight(CONTINUE);

	if (*menuState == MenuStates::EDITOR_SETUP)
		menuOptions.push_back(new MenuOption({ SAVE, textNormal->MeasureTextWidth(SAVE), height }));

	menuOptions.push_back(new MenuOption({ CONTINUE, textNormal->MeasureTextWidth(CONTINUE), height }));
	menuOptions.push_back(new MenuOption({ BACK_TO_MENU, textNormal->MeasureTextWidth(BACK_TO_MENU), height }));
	menuOptions.push_back(new MenuOption({ QUIT_GAME, textNormal->MeasureTextWidth(QUIT_GAME), height }));
}

// select a track and number of AI before starting a race
string PauseMenu::draw(GameStates& gameState)
{
	updateMousePos();
	return selectMenuOption(gameState, menuOptions, textNormal, textLarge, halfScreenWidth, halfScreenHeight - (MENU_SPACING * 2), MENU_SPACING);
}