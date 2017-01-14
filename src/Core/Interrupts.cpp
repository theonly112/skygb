#include "Interrupts.h"
#include "Instruction.h"

Interrupts::Interrupts(System* system) : system(system)
{
	master = 1;
	enable = 0;
	flags = 0;
}

void Interrupts::VBlank()
{
	system->display->DrawFramebuffer();

	master = 0;
	CommonInstructions::PushU16(system->registers->PC, system->registers.get(), system->memory.get());
	system->registers->PC = 0x40;
	system->cpu->Ticks += 12;
}

void Interrupts::LCDStat()
{
	master = 0;
	CommonInstructions::PushU16(system->registers->PC, system->registers.get(), system->memory.get());
	system->registers->PC = 0x48;
	system->cpu->Ticks += 12;
}

void Interrupts::Timer()
{
	master = 0;
	CommonInstructions::PushU16(system->registers->PC, system->registers.get(), system->memory.get());
	system->registers->PC = 0x50;
	system->cpu->Ticks += 12;
}

void Interrupts::Serial()
{
	master = 0;
	CommonInstructions::PushU16(system->registers->PC, system->registers.get(), system->memory.get());
	system->registers->PC = 0x58;
	system->cpu->Ticks += 12;
}

void Interrupts::Joypad()
{
	master = 0;
	CommonInstructions::PushU16(system->registers->PC, system->registers.get(), system->memory.get());
	system->registers->PC = 0x60;
	system->cpu->Ticks += 12;
}

void Interrupts::Step()
{
	if(master && enable && flags)
	{
		u8 fire = enable & flags;
		if(fire & InterruptVBlank)
		{
			flags &= ~InterruptVBlank;
			VBlank();
		}

		if(fire & InterruptLCDStat)
		{
			flags &= ~InterruptLCDStat;
			LCDStat();
		}

		if(fire & InterruptTimer)
		{
			flags &= ~InterruptTimer;
			Timer();
		}

		if(fire & InterruptSerial)
		{
			flags &= ~InterruptSerial;
			Serial();
		}

		if(fire &  InterruptJoypad)
		{
			flags &= InterruptJoypad;
			Joypad();
		}
	}
}
