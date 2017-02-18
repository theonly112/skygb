#include "Display.h"
#include "Gpu.h"
#include "SDL.h"
#include "SDL_opengl.h"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

Display::Display(System* system, SDL_Window* window) : system(system), window(window)
{
	
}

void Display::DrawFramebuffer() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, 1);
	glPixelZoom(1, -1);
	glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, system->gpu->framebuffer);
	SDL_GL_SwapWindow(window);
	//Sleep(1000/60);
}
