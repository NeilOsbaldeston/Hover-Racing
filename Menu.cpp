// Neil Osbaldeston

#include <TL-Engine.h>
#include "dirent.h"
#include "Menu.h"
#include "Media.h"
#include "Globals.h"

using namespace tle;

// constructor
Menu::Menu(I3DEngine* myEngine, GameMode* mode) :
	myEngine(myEngine), mode(mode), mouseX(0), mouseY(0), heightAdjustment(0), widthAdjustment(0), 
	track(""), selectedTrack(0), listPosition(0), continuePressed(false), raceAgainPressed(false)
{
	myEngine->StopMouseCapture();

	// if in fullscreen mode, need to remove the titlebar and border in the collision checks
	if (FULLSCREEN)
	{
		heightAdjustment = TITLEBAR_CORRECTION;
		widthAdjustment = BORDER_CORRECTION;
	}

	halfScreenWidth = myEngine->GetWidth() / 2;
	halfScreenHeight = myEngine->GetHeight() / 2;
	textNormal = myEngine->LoadFont(MENU_FONT, MENU_FONT_SIZE);
	textLarge = myEngine->LoadFont(MENU_FONT, MENU_FONT_SIZE_LARGE);
}

// destructor
Menu::~Menu()
{
}

// get the current location of the mouse with relation to the top left of the window/fullscreen
void Menu::updateMousePos()
{
	mouseX = myEngine->GetMouseX();
	mouseY = myEngine->GetMouseY();
}

// choose to enter a race or go back to the main menu
string Menu::selectMenuOption(GameStates& gameState, vector<MenuOption*> options, IFont* fontNorm, IFont* fontLarge,
	int textPosXOrig, int textPosYOrig, int menuSpacing, EHorizAlignment align, unsigned int colour)
{
	int heightAdjustment = 0;
	int widthAdjustment = 0;
	if (FULLSCREEN)
	{
		heightAdjustment = TITLEBAR_CORRECTION;
		widthAdjustment = BORDER_CORRECTION;
	}

	int textPosX = textPosXOrig;
	int textPosY = textPosYOrig;
	int index = 0;

	// go through each menu option
	for (auto menuIndex = options.begin(); menuIndex != options.end(); ++menuIndex)
	{
		textPosX = textPosXOrig;
		if (align == kLeft)
			textPosX += (*menuIndex)->width / 2;
		else if (align == kRight)
			textPosX -= (*menuIndex)->width / 2;

		textPosY = textPosYOrig + (index * menuSpacing);

		// if the mouse pointer is in collision with the menu options
		if (mouseX < textPosX - widthAdjustment + (*menuIndex)->width / 2 &&
			mouseX > textPosX - widthAdjustment - (*menuIndex)->width / 2 &&
			mouseY < textPosY - heightAdjustment + (*menuIndex)->height &&
			mouseY > textPosY - heightAdjustment)
		{
			// hovering over an option enlarges the font
			fontLarge->Draw((*menuIndex)->name, textPosXOrig, textPosY - (*menuIndex)->height / 4, colour, align);

			if (myEngine->KeyHit(SELECT))
			{
				// menu selections on the pause screen during a race or in the editor
				if (gameState != GameStates::MENU)
				{
					if ((*menuIndex)->name == SAVE)
					{
						mode->setSaving(true);
						continuePressed = true;
						break;
					}
					else if ((*menuIndex)->name == RACE_AGAIN)
					{
						raceAgainPressed = true;
						break;
					}
					else if ((*menuIndex)->name == CONTINUE)
					{
						// mark menu for deletion using the unpause code in HoverRacing.cpp
						continuePressed = true;
						break;
					}
					else if ((*menuIndex)->name == BACK_TO_MENU)
					{
						gameState = GameStates::MENU;
						break;
					}
					else if ((*menuIndex)->name == QUIT_GAME)
					{
						myEngine->Stop();
						break;
					}

				}
				// main menu selections
				else if (*menuState == MenuStates::MAIN_MENU)
				{
					if ((*menuIndex)->name == RACE)
					{
						*menuState = MenuStates::RACE_SETUP;
						break;
					}
					if ((*menuIndex)->name == TIME_TRIAL)
					{
						// setup time trial
						break;
					}
					else if ((*menuIndex)->name == TRACK_BUILDER)
					{
						*menuState = MenuStates::EDITOR_SETUP;
						break;
					}
					else if ((*menuIndex)->name == HIGH_SCORES)
					{
						*menuState = MenuStates::HIGH_SCORES;
						break;
					}
					else if ((*menuIndex)->name == OPTIONS)
					{
						*menuState = MenuStates::OPTIONS;
						break;
					}
					else if ((*menuIndex)->name == QUIT)
					{
						myEngine->Stop();
						break;
					}
				}
				// editor setup selections
				else if (*menuState == MenuStates::EDITOR_SETUP)
				{
					if ((*menuIndex)->name == NEW)
					{
						gameState = GameStates::PLAYING;
						break;
					}
					else if ((*menuIndex)->name == EDIT)
					{
						gameState = GameStates::PLAYING;
						return (*menuIndex)->name;
					}
					else if ((*menuIndex)->name == BACK)
					{
						gameState = GameStates::MENU;
						*menuState = MenuStates::MAIN_MENU;
						break;
					}
				}
				// race setup selections
				else if (*menuState == MenuStates::RACE_SETUP)
				{
					if ((*menuIndex)->name == START)
					{
						gameState = GameStates::PLAYING;
						break;
					}
					else if ((*menuIndex)->name == BACK)
					{
						gameState = GameStates::MENU;
						*menuState = MenuStates::MAIN_MENU;
						break;
					}
				}
				else
				{
					if ((*menuIndex)->name == BACK)
					{
						gameState = GameStates::MENU;
						*menuState = MenuStates::MAIN_MENU;
						break;
					}
				}
			}
		}
		else
			fontNorm->Draw((*menuIndex)->name, textPosXOrig, textPosY, colour, align);

		index++;
	}
	return "";
}

// extract the track list by reading the directory contents of our Track folder
bool Menu::getTrackList()
{
	// use dirent.h
	// read track directory contents

	DIR *dir;
	struct dirent *file;
	string filePath = TRACK_FILE_PATH;

	if ((dir = opendir(TRACK_FOLDER)) != NULL)
	{
		/* print all the files and directories within directory */
		while ((file = readdir(dir)) != NULL)
		{
			filePath += file->d_name;

			if (filePath.substr(filePath.length() - TRACK_EXTENSION.length(), TRACK_EXTENSION.length()) == TRACK_EXTENSION)
				tracks.push_back(filePath);

			filePath = TRACK_FILE_PATH;
		}
		closedir(dir);
	}
	else
	{
		/* could not open directory */
		//perror("");
		return false;
	}

	return true;
}

// search through a selection of tracks
void Menu::scrollTrackList()
{
	// if the mouse pointer is in collision with the track list <scroll up> arrow
	if (mouseX > trackBox->GetX() - widthAdjustment + TRACK_BOX_WIDTH - SCROLLBAR_WIDTH &&
		mouseX < trackBox->GetX() - widthAdjustment + TRACK_BOX_WIDTH &&
		mouseY > trackBox->GetY() - heightAdjustment &&
		mouseY < trackBox->GetY() - heightAdjustment + (TRACK_BOX_HEIGHT / 2))
	{
		if (listPosition > 0)
		{
			if (myEngine->KeyHit(SELECT))
				--listPosition;
		}
	}
	// if the mouse pointer is in collision with the track list <scroll down> arrow
	else if (mouseX > trackBox->GetX() - widthAdjustment + TRACK_BOX_WIDTH - SCROLLBAR_WIDTH &&
		mouseX < trackBox->GetX() - widthAdjustment + TRACK_BOX_WIDTH &&
		mouseY > trackBox->GetY() - heightAdjustment + (TRACK_BOX_HEIGHT / 2) &&
		mouseY < trackBox->GetY() - heightAdjustment + TRACK_BOX_HEIGHT)
	{
		if (listPosition < static_cast<int>(tracks.size()) - TRACKS_SHOWN_MAX)
		{
			if (myEngine->KeyHit(SELECT))
				++listPosition;
		}
	}
}

// select from the tracks available (what ever is in the Track folder)
void Menu::selectTrack()
{
	int height = listFont->MeasureTextHeight(RACE);
	int index = 0;
	unsigned int colour = kBlack;

	for (auto menuIndex = tracks.begin(); menuIndex != tracks.end(); ++menuIndex)
	{
		if (index >= listPosition && index - listPosition < TRACKS_SHOWN_MAX)
		{
			if (selectedTrack == index)
				colour = kBlue;
			else
				colour = kBlack;

			int textPosX = 15;	// offset from trackbox X pos
			int textPosY = ((index - listPosition) * height);

			// if the mouse pointer is in collision with the menu options
			if (mouseX < trackBox->GetX() - widthAdjustment + textPosX + listFont->MeasureTextWidth(tracks[index]) &&
				mouseX > trackBox->GetX() - widthAdjustment + textPosX &&
				mouseY < trackBox->GetY() - heightAdjustment + textPosY + height &&
				mouseY > trackBox->GetY() - heightAdjustment + textPosY)
			{
				if (myEngine->KeyHit(SELECT))
				{
					track = tracks[index];
					selectedTrack = index;
				}
			}
			// extract only the file name from the path name, removing all paths and extensions
			string trackName = tracks[index].substr(TRACK_FILE_PATH.length(), tracks[index].length() - TRACK_EXTENSION.length() - TRACK_FILE_PATH.length());

			listFont->Draw(trackName, static_cast<int>(trackBox->GetX()) + textPosX, static_cast<int>(trackBox->GetY()) + textPosY, colour, kLeft);
		}
		index++;
	}
}