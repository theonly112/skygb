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
	COLOUR framebuffer[160 * 144];
	System* system;
	SDL_Window* window;
public:
	int m_iWindowLine;

	Display(System* system, SDL_Window* window);
	void DrawFramebuffer() const;
	void RenderScanline();
	void RenderBackground(int line);
	void RenderWindow(int line);
	void RenderSprites(int line);
};


#endif // Display_H

