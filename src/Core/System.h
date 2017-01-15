#ifndef SYSTEM_H
#define SYSTEM_H
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>
#include <SDL.h>

#include "Cpu.h"
#include "Memory.h"
#include "Registers.h"
#include "Gpu.h"
#include "Interrupts.h"
#include "Display.h"
#include "Input.h"

class Cpu;
class IMemory;
class Memory;
class Gpu;
class Input;

class System
{
private:

public:
	System(SDL_Window* window);
	void LoadRom(const std::string& path);
	void Run();

public:
	std::shared_ptr<Cpu> cpu;
	std::shared_ptr<Memory> memory;
	std::shared_ptr<Registers> registers;
	std::shared_ptr<Gpu> gpu;
	std::shared_ptr<Interrupts> interrupts;
	std::shared_ptr<Display> display;
	std::shared_ptr<Input> input;
};

#endif