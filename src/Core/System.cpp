#include "System.h"
#include "Cartrige.h"

#include <fstream>
#include <vector>
#include <ios>



System::System(SDL_Window* window) :
	cpu(std::make_shared<Cpu>(this)),
	memory(std::make_shared<Memory>(this)),
	registers(std::make_shared<Registers>()),
	gpu(std::make_shared<Gpu>(this)),
	interrupts(std::make_shared<Interrupts>(this)),
	display(std::make_shared<Display>(this, window))
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

void System::LoadRom(const std::string& path)
{
	std::ifstream rom_stream(path.c_str(), std::ios::binary);
	if(!rom_stream.good())
	{
		throw std::runtime_error("File does not exist");
	}
	rom_stream.seekg(0, std::ios::end);
	std::streamoff length = rom_stream.tellg();
	rom_stream.seekg(0, std::ios::beg);
	std::vector<uint8_t> rom(static_cast<size_t>(length));
	rom.assign((std::istreambuf_iterator<char>(rom_stream)),
		std::istreambuf_iterator<char>());
	

	auto header = reinterpret_cast<CartrigeHeader*>(&rom[0x100]);
	auto type = static_cast<CartrigeType>(header->Cartrige_Type);
	auto romSize = 32768 << header->ROM_Size;

	IMbc* mbc;
	switch (type)
	{
	case Plain:
		mbc = new NoMbc(romSize, this);
		break;

	case MBC1:
	case MBC1RAM:
	case MBC1RAMBattery:
		mbc = new Mbc1Memory(romSize, this);
		break;

	case MBC3:
	case MBC3RAM:
	case MBC3RAMBattery:
	case MBC3TimerRAMBattery:
		mbc = new Mbc3Memory(romSize, this);
		break;
	
	case MBC5:
	case MBC5RAM:
	case MBC5RAMBattery:
	case MBC5Rumble:
	case MBC5RumbleRAM:
	case MBC5RumbleRAMBattery:
		mbc = new Mbc3Memory(romSize, this);
		break;

	default:
		throw std::runtime_error("Only plain & mbc1 cartriges supported");
	}

	mbc->LoadRom(rom);
	memory->SetMbc(mbc);
}

void System::Run()
{
	cpu->Reset();
	memory->Reset();
	gpu->Reset();
	
	bool exit = false;
	while (!exit)
	{
		SDL_Event event;
		if(SDL_PollEvent(&event))
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
					keys.keys1.a = 1;
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

		cpu->Step();
		gpu->Step();
		interrupts->Step();

		//int cyclesleft = 0;
		//for (int i = 0; i < 70224; i++) {
		//	for (int j = 0; j < 4 /** (Cpu.IsDoubleSpeed ? 2 : 1)*/; j++) {
		//		if (cyclesleft == 0) {
		//			cpu->Step();
		//			//cyclesleft = Cpu.Tick();
		//		}
		//		cyclesleft--;
		//	}

		//	gpu->Step();
		//	interrupts->Step();
		//}
	}
}



