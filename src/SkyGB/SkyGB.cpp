#include "Core/System.h"

#include <iostream>

#include "SDL.h"

#undef main


SDL_Window* CreateWindow()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::exception("Could not initialize SDL");
	}
	SDL_Window* window = SDL_CreateWindow("SkyGb",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		160, 144,
		SDL_WINDOW_OPENGL);
		
	if(window == nullptr)
	{
		throw std::exception("Could not create window");
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(context == nullptr)
	{
		throw std::exception("Could not create GL context");
	}
	return window;
}

int main()
{
	try
	{
	
		auto window = CreateWindow();
		System s(window);
		//s.LoadRom("../../TestRoms/cpu_instrs.gb");
		//s.LoadRom("../../../TestRoms/tetris.gb");
		//s.LoadRom("../../TestRoms/Pokemon - Red Version (USA, Europe).gb");
		//s.LoadRom("../../../../TestRoms/tetris.gb");
		s.LoadRom("./TestRoms/tetris.gb");
		s.Run();

	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cin.get();
	}
	return 0;
}