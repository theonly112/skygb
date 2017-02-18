#ifndef Display_H
#define Display_H
#include "Defines.h"
#include <SDL.h>

class System;

const COLOUR palette[4] = {
	{ 255, 255, 255 },
	{ 192, 192, 192 },
	{ 96, 96, 96 },
	{ 0, 0, 0 },
};

class Display
{
	System* system;
	SDL_Window* window;
public:
	Display(System* system, SDL_Window* window);
	void DrawFramebuffer() const;
};


#endif // Display_H

