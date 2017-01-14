#ifndef REGISTERS_H
#define REGISTERS_H

#include "Defines.h"

struct SixteenBitRegister
{
	union
	{
		struct
		{
			u8 Low;
			u8 High;
		};
		u16 Value;
	};
};

enum RegisterNames
{
	A,
	F,
	AF,
	B,
	C,
	BC,
	D,
	E,
	DE,
	H,
	L,
	HL,
	SP,
	PC,
};


enum Flags
{
	Carry = 1 << 4,
	HalfCarry = 1 << 5,
	Negative = 1 << 6,
	Zero = 1 << 7
};

inline Flags operator|(Flags a, Flags b)
{
	return static_cast<Flags>(static_cast<int>(a) | static_cast<int>(b));
}


class Registers
{
public:
	SixteenBitRegister AF;
	SixteenBitRegister BC;
	SixteenBitRegister DE;
	SixteenBitRegister HL;
	u16 SP;
	u16 PC;

	void Set(Flags flag)
	{
		AF.Low |= flag;
	}

	bool IsSet(Flags flag) const
	{
		return (AF.Low & flag) > 0;
	}

	void Clear(Flags flag)
	{
		AF.Low &= ~flag;
	}
};

#endif