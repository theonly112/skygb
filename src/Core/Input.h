#ifndef INPUT_H
#define INPUT_H
#include <SDL.h>
#include "Defines.h"
#include "Cpu.h"

class Cpu;
class Input
{
public:
    Input(System* system);
    ~Input();
    keys keys;
    void Step();
private:
    System* system;
};

#endif