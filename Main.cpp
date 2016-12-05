// Hover Racing: A program using the TL-Engine
// Author: Neil Osbaldeston


#include "HoverRacing.h"

void main()
{
	// create a game
	HoverRacing* game = new HoverRacing;

	// run the game loop
	game->runGame();

	// release resources
	game->cleanUp();
}