#include "Core/System.h"

#include <iostream>

#include "SDL.h"

#undef main


SDL_Window* CreateWindow()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::runtime_error("Could not initialize SDL");
	}
	SDL_Window* window = SDL_CreateWindow("SkyGb",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		160, 144,
		SDL_WINDOW_OPENGL);
		
	if(window == nullptr)
	{
		throw std::runtime_error("Could not create window");
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(context == nullptr)
	{
		throw std::runtime_error("Could not create GL context");
	}
	return window;
}

int main(int argc, char** argv)
{
	try
	{
		if(argc < 2)
		{
			std::cout << "Please provide the path to a rom as argument" << std::endl;
			return 0;
		}
		auto window = CreateWindow();
		System s(window);
		s.LoadRom(argv[1]);	
		s.Run();

	}
	catch(std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
		std::cin.get();
	}
	return 0;
}