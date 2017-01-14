#ifndef Interrupts_H
#define Interrupts_H
#include "Defines.h"

class System;

enum Interrupt
{
	InterruptVBlank = (1 << 0),
	InterruptLCDStat = (1 << 1),
	InterruptTimer = (1 << 2),
	InterruptSerial = (1 << 3),
	InterruptJoypad = (1 << 4)
};

class Interrupts
{
	System* system;
public:
	explicit Interrupts(System* system);
	u8 master;
	u8 enable;
	u8 flags;

	void VBlank();
	void LCDStat();
	void Timer();
	void Serial();
	void Joypad();
	void Step();
};

#endif // Interrupts_H

