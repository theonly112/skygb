#ifndef WRAPPER_H
#define WRAPPER_H

#include "../System.h"
#include <string>

extern "C" {

struct HSystem;
typedef struct HSystem HSystem;

#define get(S) reinterpret_cast<System*>(system)

__declspec(dllexport) HSystem* SkyGB_Create()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return nullptr;
    }
    SDL_Window *window = SDL_CreateWindow("SkyGb",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160, 144, SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        return nullptr;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr)
    {
        return nullptr;
    }

    System* s = new System(window);
    return reinterpret_cast<HSystem *>(s);
}

__declspec(dllexport) bool SkyGB_Load(HSystem *system, const char *rom)
{
    std::string s(rom);
	get(system)->LoadRom(s);
    return true;
}

__declspec(dllexport) bool SkyGB_Run(HSystem *system)
{
    get(system)->Run();
    return true;
}

}

#endif