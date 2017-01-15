#include "Input.h"

Input::Input(System* system)
{
    this->system = system;
}

Input::~Input()
{
    keys.keys1.a = 1;
    keys.keys1.b = 1;
    keys.keys1.select = 1;
    keys.keys1.start = 1;
    keys.keys2.right = 1;
    keys.keys2.left = 1;
    keys.keys2.up = 1;
    keys.keys2.down = 1;
}

void Input::Step()
{
    auto cpu = system->cpu;
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
        {
            SDL_KeyboardEvent key = event.key;
            switch (key.keysym.sym)
            {
            case SDLK_LEFT:
                keys.keys2.left = 0;
                cpu->Stopped = false;
                break;
            case SDLK_RIGHT:
                keys.keys2.right = 0;
                cpu->Stopped = false;
                break;
            case SDLK_UP:
                keys.keys2.up = 0;
                cpu->Stopped = false;
                break;
            case SDLK_DOWN:
                keys.keys2.down = 0;
                cpu->Stopped = false;
                break;
            case SDLK_a:
                keys.keys1.a = 0;
                cpu->Stopped = false;
                break;
            case SDLK_b:
                keys.keys1.b = 0;
                cpu->Stopped = false;
                break;
            case SDLK_SPACE:
                keys.keys1.start = 0;
                cpu->Stopped = false;
                break;
            case SDLK_BACKSPACE:
                keys.keys1.select = 0;
                cpu->Stopped = false;
                break;
            default:
                break;
            }
            break;
        }

        case SDL_KEYUP:
        {
            SDL_KeyboardEvent key = event.key;
            switch (key.keysym.sym)
            {
            case SDLK_LEFT:
                keys.keys2.left = 1;
                cpu->Stopped = false;
                break;
            case SDLK_RIGHT:
                keys.keys2.right = 1;
                cpu->Stopped = false;
                break;
            case SDLK_UP:
                keys.keys2.up = 1;
                cpu->Stopped = false;
                break;
            case SDLK_DOWN:
                keys.keys2.down = 1;
                cpu->Stopped = false;
                break;
            case SDLK_a:
                keys.keys1.a = 1;
                cpu->Stopped = false;
                break;
            case SDLK_b:
                keys.keys1.b = 1;
                cpu->Stopped = false;
                break;
            case SDLK_SPACE:
                keys.keys1.start = 1;
                cpu->Stopped = false;
                break;
            case SDLK_BACKSPACE:
                keys.keys1.select = 1;
                cpu->Stopped = false;
                break;
            default:
                break;
            }
            break;
        }

        default:
            break;
        }
    }
}