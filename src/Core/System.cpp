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
	display(std::make_shared<Display>(this, window)),
	input(std::make_shared<Input>(this))
{

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
		cpu->Step();
		gpu->Step();
		interrupts->Step();
		input->Step();

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



