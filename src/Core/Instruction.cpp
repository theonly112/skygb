#include "Instruction.h"

void CommonInstructions::Xor(u8 value, Registers* registers)
{
	registers->AF.High ^= value;

	if (registers->AF.High) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Carry | Negative | HalfCarry);
}

void CommonInstructions::PushU16(u16 value, Registers* registers, IMemory* memory)
{
	registers->SP -= 2;
	memory->WriteWord(registers->SP, value);
}

void CommonInstructions::Sub(u8 value, Registers* registers)
{
	registers->Set(Negative);
	
	if (value > registers->AF.High) registers->Set(Carry);
	else registers->Clear(Carry);

	if ((value & 0x0f) > (registers->AF.High & 0x0f)) 
		registers->Set(HalfCarry);
	else 
		registers->Clear(HalfCarry);

	registers->AF.High -= value;

	if (registers->AF.High) registers->Clear(Zero);
	else registers->Set(Zero);
}

u16 CommonInstructions::PopU16(Registers* registers, IMemory* memory)
{
	u16 value = memory->ReadWord(registers->SP);
	registers->SP += 2;
	return value;
}

void CommonInstructions::RLC(u8* reg8, Registers* registers)
{
	u8 value = *reg8;
	int32_t carry = (value & 0x80) >> 7;

	if (value & 0x80) registers->Set(Carry);
	else registers->Clear(Carry);
	
	value <<= 1;
	value += carry;

	if (value) registers->Clear(Zero);
	else registers->Clear(Zero);

	registers->Clear(Negative | HalfCarry);

	*reg8 = value;
}

void CommonInstructions::Sbc(u8 value, Registers* registers)
{
	value += registers->IsSet(Carry) ? 1 : 0;

	registers->Set(Negative);

	if (value > registers->AF.High) registers->Set(Carry);
	else registers->Clear(Carry);

	if (value == registers->AF.High) registers->Set(Zero);
	else registers->Clear(Zero);

	if ((value & 0x0F) > (registers->AF.High & 0x0F)) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);

	registers->AF.High -= value;
}

void CommonInstructions::Or(u8 value, Registers* registers)
{
	registers->AF.High |= value;

	if (registers->AF.High) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Carry | Negative | HalfCarry);
}

void CommonInstructions::Add(u8* destination, u8 value, Registers* registers)
{
	uint32_t result = *destination + value;

	if (result & 0xFF00) registers->Set(Carry);
	else registers->Clear(Carry);

	*destination = static_cast<u8>(result & 0xff);

	if (*destination) registers->Clear(Zero);
	else registers->Set(Zero);

	if (((*destination & 0x0f) + (value & 0x0f)) > 0x0f) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);

	registers->Clear(Negative);
}

void CommonInstructions::Cp(u8 value, Registers* registers)
{
	if (registers->AF.High == value) registers->Set(Zero);
	else registers->Clear(Zero);

	if (value > registers->AF.High) registers->Set(Carry);
	else registers->Clear(Carry);

	if ((value & 0x0f) > (registers->AF.High & 0x0f)) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);

	registers->Set(Negative);
}

void CommonInstructions::SWAP(u8* reg8, Registers* registers)
{
	u8 value = *reg8;

	value = ((value & 0xf) << 4) | ((value & 0xf0) >> 4);

	if (value) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Negative | HalfCarry | Carry);

	*reg8 = value;
}

void CommonInstructions::And(u8 value, Registers* registers)
{
	registers->AF.High &= value;
	
	if (registers->AF.High) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Carry | Negative);
	registers->Set(HalfCarry);
}

void CommonInstructions::Inc(u8* reg, Registers* registers)
{
	u8 value = *reg;

	if ((value & 0x0f) == 0x0f) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);

	value++;

	if (value) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Negative);

	*reg = value;
}

void CommonInstructions::Sla(u8* reg8, Registers* registers)
{
	u8 value = *reg8;

	if (value & 0x80) registers->Set(Carry);
	else registers->Clear(Carry);

	value <<= 1;

	if (value) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Negative | HalfCarry);

	*reg8 = value;
}

void CommonInstructions::Adc(uint8_t n, Registers* registers)
{
	n += registers->IsSet(Carry) ? 1 : 0;

	int32_t result = registers->AF.High + n;

	if (result & 0xff00) registers->Set(Carry);
	else registers->Clear(Carry);

	if (n == registers->AF.High) registers->Set(Zero);
	else registers->Clear(Zero);

	if (((n & 0x0f) + (registers->AF.High & 0x0f)) > 0x0f) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);

	registers->Set(Negative);

	registers->AF.High = static_cast<uint8_t>(result & 0xff);
}

void CommonInstructions::Rr(u8* r8, Registers* registers)
{
	uint8_t value = *r8;
	value >>= 1;

	if (registers->IsSet(Carry)) 
		value |= 0x80;

	if (value & 0x01) registers->Set(Carry);
	else registers->Clear(Carry);

	if (value) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Negative | HalfCarry);

	*r8 = value;
}

void CommonInstructions::Rl(u8* reg8, Registers* registers)
{
	uint8_t value = *reg8;
	int32_t carry = registers->IsSet(Carry) ? 1 : 0;

	if (value & 0x80) registers->IsSet(Carry);
	else registers->Clear(Carry);

	value <<= 1;
	value += carry;

	if (value) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Negative | HalfCarry);

	*reg8 = value;
}

void CommonInstructions::Srl(u8* reg8, Registers* registers)
{
	uint8_t value = *reg8;
	
	if (value & 0x01) registers->Set(Carry);
	registers->Clear(Carry);


	value >>= 1;

	if (value) registers->Clear(Zero);
	registers->Set(Zero);

	registers->Clear(Negative | HalfCarry);

	*reg8 = value;
}

void CommonInstructions::Dec(u8* reg8, Registers* registers)
{
	uint8_t value = *reg8;

	if (value & 0x0f) registers->Clear(HalfCarry);
	else registers->Set(HalfCarry);

	value--;

	if (value) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Set(Negative);

	*reg8 = value;
}

void CommonInstructions::Bit(u8 bit, u8 value, Registers* registers)
{
	if (value & bit) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Negative);
	registers->Set(HalfCarry);
}

void CommonInstructions::Rrc(uint8_t* ptr, Registers* registers)
{
	uint8_t value = *ptr;
	int32_t carry = value & 0x01;

	value >>= 1;

	if(carry)
	{
		registers->Set(Carry);
		value |= 0x80;
	}
	else registers->Clear(Carry);

	if (value) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Negative | HalfCarry);

	*ptr = value;
}

void CommonInstructions::Sra(u8* ptr, Registers* registers)
{
	auto value = *ptr;

	if (value & 0x01) registers->Set(Carry);
	else registers->Clear(Carry);

	value = (value & 0x80) | (value >> 1);

	if (value) registers->Clear(Zero);
	else registers->Set(Zero);

	registers->Clear(Negative | HalfCarry);

	*ptr = value;
}

void LD_Reg16_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 value = memory->ReadWord(registers->PC);
	registers->PC += 2;
	switch (m_reg)
	{
	case BC:
		Execute(&registers->BC, value);
		break;
	case DE:
		Execute(&registers->DE, value);
		break;
	case HL:
		Execute(&registers->HL, value);
		break;
	case SP:
		Execute(&registers->SP, value);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
	
}

void LD_Reg16_NN::Execute(SixteenBitRegister* reg, u16 val)
{
	Execute(&reg->Value, val);
}

void LD_Reg16_NN::Execute(u16* reg, u16 val)
{
	*reg = val;
}

void LD_Reg16Ptr_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg16)
	{
	case BC:
		Execute(memory, &registers->BC, registers->AF.High);
		break;
	case DE:
		Execute(memory, &registers->DE, registers->AF.High);
		break;
	case HL:
	{
		switch (m_reg8)
		{
		case B:
			Execute(memory, &registers->HL, registers->BC.High);
			break;
		case C:
			Execute(memory, &registers->HL, registers->BC.Low);
			break;
		case D:
			Execute(memory, &registers->HL, registers->DE.High);
			break;
		case E:
			Execute(memory, &registers->HL, registers->DE.Low);
			break;
		case H:
			Execute(memory, &registers->HL, registers->HL.High);
			break;
		case L:
			Execute(memory, &registers->HL, registers->HL.Low);
			break;
		case A:
			Execute(memory, &registers->HL, registers->AF.High);
			break;
		default:
			throw std::runtime_error("Not implemented");
		}
	}
	break;
	default:
		throw std::runtime_error("Not implemented");
	}
}

void LD_Reg16Ptr_Reg8::Execute(IMemory* memory, SixteenBitRegister* reg16, u8 reg8Value)
{
	memory->WriteByte(reg16->Value, reg8Value);
}

void INC_Reg16::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case BC:
		Execute(&registers->BC);
		break;
	case DE:
		Execute(&registers->DE);
		break;
	case HL:
		Execute(&registers->HL);
		break;
	case SP:
		Execute(&registers->SP);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void INC_Reg16::Execute(SixteenBitRegister* reg)
{
	Execute(&reg->Value);
}

void INC_Reg16::Execute(u16* reg)
{
	*reg += 1;
}

void INC_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		Execute(&registers->BC.High, registers);
		break;
	case C:
		Execute(&registers->BC.Low, registers);
		break;
	case D:
		Execute(&registers->DE.High, registers);
		break;
	case E:
		Execute(&registers->DE.Low, registers);
		break;
	case H:
		Execute(&registers->HL.High, registers);
		break;
	case L:
		Execute(&registers->HL.Low, registers);
		break;
	case A:
		Execute(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void INC_Reg8::Execute(u8* reg, Registers* registers)
{
	CommonInstructions::Inc(reg, registers);
	
}

void DEC_Reg16::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case BC:
		Execute(&registers->BC.Value);
		break;
	case DE:
		Execute(&registers->DE.Value);
		break;
	case HL:
		Execute(&registers->HL.Value);
		break;
	case SP:
		Execute(&registers->SP);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void DEC_Reg16::Execute(u16* reg)
{
	*reg -= 1;
}

void DEC_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		Execute(&registers->BC.High, registers);
		break;
	case C:
		Execute(&registers->BC.Low, registers);
		break;
	case D:
		Execute(&registers->DE.High, registers);
		break;
	case E:
		Execute(&registers->DE.Low, registers);
		break;
	case H:
		Execute(&registers->HL.High, registers);
		break;
	case L:
		Execute(&registers->HL.Low, registers);
		break;
	case A:
		Execute(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid registers");
	}
}

void DEC_Reg8::Execute(u8* reg, Registers* regs)
{
	u8 value = *reg;
	if (value & 0x0f) regs->Clear(HalfCarry);
	else regs->Set(HalfCarry);

	value--;

	if (value) regs->Clear(Zero);
	else regs->Set(Zero);

	regs->Set(Negative);

	*reg = value;
}

void LD_Reg8_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 value = memory->ReadByte(registers->PC);
	registers->PC += 1;
	switch (m_reg)
	{
	case B:
		Execute(&registers->BC.High, value);
		break;
	case C: 
		Execute(&registers->BC.Low, value);
		break;
	case D:
		Execute(&registers->DE.High, value);
		break;
	case E:
		Execute(&registers->DE.Low, value);
		break;
	case H:
		Execute(&registers->HL.High, value);
		break;
	case L:
		Execute(&registers->HL.Low, value);
		break;
	case A:
		Execute(&registers->AF.High, value);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void LD_Reg8_N::Execute(u8* reg, u8 value)
{
	*reg = value;
}

void RLCA::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 carry = (registers->AF.High & 0x80) >> 7;
	if (carry) registers->Set(Carry);
	else registers->Clear(Carry);

	registers->AF.High <<= 1;
	registers->AF.High += carry;

	registers->Clear(Negative | Zero | HalfCarry);
}

void LD_NNPtr_SP::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC += 2;
	memory->WriteWord(addr, registers->SP);
}

void ADD_HL_Reg16::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case BC:
		Execute(&registers->HL, &registers->BC, registers);
		break;
	case DE:
		Execute(&registers->HL, &registers->DE, registers);
		break;
	case HL:
		Execute(&registers->HL, &registers->HL, registers);
		break;
	case SP:
		Execute(&registers->HL, registers->SP, registers);
		break;
	default:
		throw std::runtime_error("Not implemented");
	}
}

void ADD_HL_Reg16::Execute(SixteenBitRegister* regHL, SixteenBitRegister* reg16, Registers* registers)
{
	Execute(regHL, reg16->Value, registers);
}

void ADD_HL_Reg16::Execute(SixteenBitRegister* regHL, u16 reg16Value, Registers* registers)
{
	uint32_t result = regHL->Value + reg16Value;

	if (result & 0xffff0000) registers->Set(Carry);
	else registers->Clear(Carry);

	regHL->Value = static_cast<u16>(result & 0xffff);

	if (((regHL->Value & 0x0f) + (reg16Value & 0x0F)) > 0x0f) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);

	registers->Clear(Negative);
}

void LD_Reg8_Reg16Ptr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	SixteenBitRegister* reg16;

	switch (m_reg16)
	{
	case BC:
		reg16 = &registers->BC;
		break;
	case DE:
		reg16 = &registers->DE;
		break;
	case HL:
		reg16 = &registers->HL;
		break;
	default:
		throw std::runtime_error("Invalid reg16");
	}

	switch (m_reg8)
	{
	case A:
		Execute(memory, &registers->AF.High, reg16);
		break;
	case B:
		Execute(memory, &registers->BC.High, reg16);
		break;
	case C:
		Execute(memory, &registers->BC.Low, reg16);
		break;
	case D:
		Execute(memory, &registers->DE.High, reg16);
		break;
	case E:
		Execute(memory, &registers->DE.Low, reg16);
		break;
	case H:
		Execute(memory, &registers->HL.High, reg16);
		break;
	case L:
		Execute(memory, &registers->HL.Low, reg16);
		break;
	default:
		throw std::runtime_error("Not implemented");
	}
}

void LD_Reg8_Reg16Ptr::Execute(IMemory* memory, u8* regA, SixteenBitRegister* regB)
{
	*regA = memory->ReadByte(regB->Value);
}

void RRCA::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 carry = registers->AF.High & 0x01;
	
	if (carry) registers->Set(Carry);
	else registers->Clear(Carry);

	registers->AF.High >>= 1;

	if (carry) registers->AF.High |= 0x80;

	registers->Clear(Negative | Zero | HalfCarry);
}

void STOP::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	cpu->Stopped = true;
}

void RLA::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	int carry = registers->IsSet(Carry) ? 1 : 0;

	if (registers->AF.High & 0x80) registers->Set(Carry);
	else registers->Clear(Carry);

	registers->AF.High <<= 1;
	registers->AF.High += carry;

	registers->Clear(Negative | Zero | HalfCarry);
}

void JR_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	int8_t relativeAddr = memory->ReadByte(registers->PC);
	registers->PC += 1;
	registers->PC += relativeAddr;
}

void RRA::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	int carry = (registers->IsSet(Carry) ? 1 : 0) << 7;

	if (registers->AF.High & 0x01) registers->Set(Carry);
	else registers->Clear(Carry);

	registers->AF.High >>= 1;
	registers->AF.High &= carry;

	registers->Clear(Negative | Zero | HalfCarry);
}

void JR_NZ_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 value = memory->ReadByte(registers->PC);
	registers->PC += 1;

	if(registers->IsSet(Zero))
	{
		cpu->Ticks += 8;
	}
	else
	{
		registers->PC += static_cast<int8_t>(value);
		cpu->Ticks += 12;
	}
}

void LDI_HLPtr_A::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	memory->WriteByte(registers->HL.Value++, registers->AF.High);
}

void DAA::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 s = registers->AF.High;

	if(registers->IsSet(Negative))
	{
		if (registers->IsSet(HalfCarry)) s = (s - 0x06) & 0xFF;
		if (registers->IsSet(Carry)) (s -= 0x60);
	}
	else
	{
		if (registers->IsSet(HalfCarry) || (s & 0xf) > 9) s += 0x06;
		if (registers->IsSet(Carry) || s > 0x9F) s += 0x60;
	}

	registers->AF.High = static_cast<u8>(s);
	registers->Clear(HalfCarry);

	if (registers->AF.High) registers->Clear(Zero);
	else registers->Set(Zero);

	if (s >= 0x100) registers->Set(Carry);

}

void JR_Z_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	int8_t relativeAddr = memory->ReadByte(registers->PC);
	registers->PC++;
	if(registers->IsSet(Zero))
	{
		registers->PC += relativeAddr;
		cpu->Ticks += 12;
	}
	else
	{
		cpu->Ticks += 8;
	}
}

void JR_NC_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	int8_t relativeJmp = memory->ReadByte(registers->PC);
	registers->PC++;
	if(registers->IsSet(Carry))
	{
		cpu->Ticks += 8;
	}
	else
	{
		registers->PC += relativeJmp;
		cpu->Ticks += 12;
	}
}

void LDD_HLPtr_A::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	memory->WriteByte(registers->HL.Value, registers->AF.High);
	registers->HL.Value--;
}

void INC_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 hlValue = memory->ReadByte(registers->HL.Value);
	CommonInstructions::Inc(&hlValue, registers);
	memory->WriteByte(registers->HL.Value, hlValue);
}

void DEC_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 hlValue = memory->ReadByte(registers->HL.Value);
	CommonInstructions::Dec(&hlValue, registers);
	memory->WriteByte(registers->HL.Value, hlValue);
}

void LD_HLPtr_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 n = memory->ReadByte(registers->PC);
	registers->PC++;
	memory->WriteByte(registers->HL.Value, n);
}

void SCF::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	registers->Set(Carry);
	registers->Clear(Negative | HalfCarry);
}

void JR_C_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	int8_t relativeJmp = memory->ReadByte(registers->PC);
	registers->PC++;
	if(registers->IsSet(Carry))
	{
		registers->PC += relativeJmp;
		cpu->Ticks += 12;
	}
	else
	{
		cpu->Ticks += 8;
	}
}

void LD_Reg8_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8* regL;
	switch (m_regL)
	{
	case A:
		regL = &registers->AF.High;
		break;
	case B:
		regL = &registers->BC.High;
		break;
	case C:
		regL = &registers->BC.Low;
		break;
	case D:
		regL = &registers->DE.High;
		break;
	case E:
		regL = &registers->DE.Low;
		break;
	case H:
		regL = &registers->HL.High;
		break;
	case L:
		regL = &registers->HL.Low;
		break;
	default:
		throw std::runtime_error("Invalid left register");
	}

	u8 regRValue;
	switch (m_regR)
	{
	case A:
		regRValue = registers->AF.High;
		break;
	case B:
		regRValue = registers->BC.High;
		break;
	case C:
		regRValue = registers->BC.Low;
		break;
	case D:
		regRValue = registers->DE.High;
		break;
	case E:
		regRValue = registers->DE.Low;
		break;
	case H:
		regRValue = registers->HL.High;
		break;
	case L:
		regRValue = registers->HL.Low;
		break;
	default:
		throw std::runtime_error("Invalid left register");
	}

	*regL = regRValue;
}

void LDI_A_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	registers->AF.High = memory->ReadByte(registers->HL.Value++);
}

void CPL::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	registers->AF.High = ~registers->AF.High;
}

void LDD_A_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	registers->AF.High = memory->ReadByte(registers->HL.Value--);
}

void CCF::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	if (registers->IsSet(Carry)) registers->Clear(Carry);
	else registers->Set(Carry);

	registers->Clear(Negative | HalfCarry);
}

void HALT::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	if(interrupts->master)
	{
		cpu->Halted = true;
	}
	else registers->PC++;
}

void ADD_A_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8* destination = &registers->AF.High;
	u8 value;

	switch (m_reg)
	{
	case A:
		value = registers->AF.High;
		break;
	case B:
		value = registers->BC.High;
		break;
	case C:
		value = registers->BC.Low;
		break;
	case D:
		value = registers->DE.High;
		break;
	case E:
		value = registers->DE.Low;
		break;
	case H:
		value = registers->HL.High;
		break;
	case L:
		value = registers->HL.Low;
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
	CommonInstructions::Add(destination, value, registers);
}

void ADD_A_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 value = memory->ReadByte(registers->HL.Value);
	CommonInstructions::Add(&registers->AF.High, value, registers);
}

void ADC_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case A:
		CommonInstructions::Adc(registers->AF.High, registers);
		break;
	case B:
		CommonInstructions::Adc(registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Adc(registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Adc(registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Adc(registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Adc(registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Adc(registers->HL.Low, registers);
		break;
	default:
		throw std::runtime_error("Invalid instruction");
	}
}

void ADC_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 value = memory->ReadByte(registers->HL.Value);
	CommonInstructions::Adc(value, registers);
}

void SUB_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch(m_reg)
	{
	case A:
		CommonInstructions::Sub(registers->AF.High, registers);
		break;
	case B:
		CommonInstructions::Sub(registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Sub(registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Sub(registers->DE.High, registers);
		break;
	case E: 
		CommonInstructions::Sub(registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Sub(registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Sub(registers->HL.Low, registers);
		break;
	default:
		throw std::runtime_error("Invalid instruction");
	}
}

void SUB_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	CommonInstructions::Sub(memory->ReadByte(registers->HL.Value), registers);
}

void SBC_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case A:
		CommonInstructions::Sbc(registers->AF.High, registers);
		break;
	case B:
		CommonInstructions::Sbc(registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Sbc(registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Sbc(registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Sbc(registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Sbc(registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Sbc(registers->HL.Low, registers);
		break;
	default:
		throw std::runtime_error("Invalid instruction");
	}
}

void SBC_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	CommonInstructions::Sbc(memory->ReadByte(registers->HL.Value), registers);
}

void AND_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::And(registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::And(registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::And(registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::And(registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::And(registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::And(registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::And(registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void AND_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	auto value = memory->ReadByte(registers->HL.Value);
	CommonInstructions::And(value, registers);
}

void XOR_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Xor(registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Xor(registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Xor(registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Xor(registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Xor(registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Xor(registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Xor(registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void XOR_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	CommonInstructions::Xor(memory->ReadByte(registers->HL.Value), registers);
}

void OR_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Or(registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Or(registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Or(registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Or(registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Or(registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Or(registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Or(registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void OR_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	CommonInstructions::Or(memory->ReadByte(registers->HL.Value), registers);
}

void CP_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Cp(registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Cp(registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Cp(registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Cp(registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Cp(registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Cp(registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Cp(registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void CP_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint8_t n = memory->ReadByte(registers->HL.Value);
	CommonInstructions::Cp(n, registers);
}

void RET_NZ::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	if(registers->IsSet(Zero))
	{
		cpu->Ticks += 8;
	}
	else
	{
		registers->PC = CommonInstructions::PopU16(registers, memory);
		cpu->Ticks += 20;
	}
}

void POP_Reg16::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case BC:
		registers->BC.Value = CommonInstructions::PopU16(registers, memory);
		break;
	case DE:
		registers->DE.Value = CommonInstructions::PopU16(registers, memory);
		break;
	case HL:
		registers->HL.Value = CommonInstructions::PopU16(registers, memory);
		break;
	case AF:
		registers->AF.Value = CommonInstructions::PopU16(registers, memory);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void JP_NZ_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC += 2;

	if(registers->IsSet(Zero))
	{
		cpu->Ticks += 12;
	}
	else
	{
		registers->PC = addr;
		cpu->Ticks += 16;
	}
}

void JP_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC = addr;
}

void CALL_NZ_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint16_t addr = memory->ReadWord(registers->PC);
	registers->PC += 2;

	if(registers->IsSet(Zero))
	{
		cpu->Ticks += 12;
	}
	else
	{
		CommonInstructions::PushU16(registers->PC, registers, memory);
		registers->PC = addr;
		cpu->Ticks += 24;
	}
}

void PUSH_Reg16::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case AF:
		CommonInstructions::PushU16(registers->AF.Value, registers, memory);
		break;
	case BC:
		CommonInstructions::PushU16(registers->BC.Value, registers, memory);
		break;
	case DE:
		CommonInstructions::PushU16(registers->DE.Value, registers, memory);
		break;
	case HL:
		CommonInstructions::PushU16(registers->HL.Value, registers, memory);
		break;
	default:
		throw std::runtime_error("Not implemented");
	}
}

void ADD_A_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 N = memory->ReadByte(registers->PC);
	registers->PC++;

	CommonInstructions::Add(&registers->AF.High, N, registers);
}

void RST::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	CommonInstructions::PushU16(registers->PC, registers, memory);
	registers->PC = this->rst_value;
}

void RET_Z::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	if(registers->IsSet(Zero))
	{
		registers->PC = CommonInstructions::PopU16(registers, memory);
		cpu->Ticks += 20;
	}
	else
	{
		cpu->Ticks += 8;
	}
}

void RET::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	registers->PC = CommonInstructions::PopU16(registers, memory);
}

void JP_Z_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC += 2;

	if(registers->IsSet(Zero))
	{
		registers->PC = addr;
		cpu->Ticks += 16;
	}
	else
	{
		cpu->Ticks += 12;
	}
}

void CB_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	throw std::runtime_error("Not implemented");
}

void CALL_Z_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC += 2;

	if(registers->IsSet(Zero))
	{
		CommonInstructions::PushU16(registers->PC, registers, memory);
		registers->PC = addr;
		cpu->Ticks += 24;
	}
	else
	{
		cpu->Ticks += 12;
	}
}

void CALL_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC += 2;
	CommonInstructions::PushU16(registers->PC, registers, memory);
	registers->PC = addr;
}

void ADC_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint8_t n = memory->ReadByte(registers->PC);
	registers->PC++;
	CommonInstructions::Adc(n, registers);
}

void RET_NC::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	if(registers->IsSet(Carry))
	{
		cpu->Ticks += 8;
	}
	else
	{
		registers->PC = CommonInstructions::PopU16(registers, memory);
		cpu->Ticks += 20;
	}
}

void JP_NC_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC += 2;

	if (registers->IsSet(Carry))
	{
		cpu->Ticks += 12;
	}
	else
	{
		registers->PC = addr;
		cpu->Ticks += 16;
	}
}

void UNKNOWN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	throw std::runtime_error("Unknown Instruction");
}

void CALL_NC_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint16_t n = memory->ReadWord(registers->PC);
	registers->PC += 2;

	if(registers->IsSet(Carry))
	{
		cpu->Ticks += 12;
	}
	else
	{
		CommonInstructions::PushU16(registers->PC, registers, memory);
		registers->PC = n;
		cpu->Ticks += 24;
	}
	
}

void SUB_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint8_t n = memory->ReadByte(registers->PC);
	registers->PC++;
	CommonInstructions::Sub(n, registers);
}

void RET_C::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	if(registers->IsSet(Carry))
	{
		registers->PC = CommonInstructions::PopU16(registers, memory);
		cpu->Ticks += 20;
	}
	else
	{
		cpu->Ticks += 8;
	}
}

void RETI::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	interrupts->master = 1;
	registers->PC = CommonInstructions::PopU16(registers, memory);
}

void JP_C_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint16_t addr = memory->ReadWord(registers->PC);
	registers->PC += 2;

	if(registers->IsSet(Carry))
	{
		registers->PC = addr;
		cpu->Ticks += 16;
	}
	else
	{
		cpu->Ticks += 12;
	}
}

void CALL_C_NN::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint16_t addr = memory->ReadWord(registers->PC);
	registers->PC += 2;
	if(registers->IsSet(Carry))
	{
		CommonInstructions::PushU16(registers->PC, registers, memory);
		registers->PC = addr;
		cpu->Ticks += 24;
	}
	else
	{
		cpu->Ticks += 12;
	}
}

void SBC_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 n = memory->ReadByte(registers->PC);
	registers->PC++;
	CommonInstructions::Sbc(n, registers);
}

void LD_FF_N_AP::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 addr = memory->ReadByte(registers->PC);
	registers->PC += 1;
	memory->WriteByte(0xFF00 + addr, registers->AF.High);
}

void LD_FF_C_A::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	memory->WriteByte(0xFF00 + registers->BC.Low, registers->AF.High);
}

void AND_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 n = memory->ReadByte(registers->PC);
	registers->PC++;

	registers->AF.High &= n;

	registers->Clear(Carry | Negative);
	registers->Set(HalfCarry);

	if (registers->AF.High) registers->Clear(Zero);
	else registers->Set(Zero);
}

void ADD_SP_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	int8_t n = static_cast<int8_t>(memory->ReadByte(registers->PC));
	registers->PC++;

	int32_t result = registers->SP + n;

	if (result & 0xffff0000) registers->Set(Carry);
	else registers->Clear(Carry);

	registers->SP = result & 0xFFFF;

	if (((registers->SP & 0x0f) + (n & 0x0f)) > 0x0f) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);

	registers->Clear(Zero | Negative);
}

void JP_HL::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	registers->PC = registers->HL.Value;
}

void LD_NNPtr_A::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC += 2;
	memory->WriteByte(addr, registers->AF.High);
}

void XOR_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint8_t n = memory->ReadByte(registers->PC);
	registers->PC++;

	CommonInstructions::Xor(n, registers);
}

void LD_FF_APtr_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 addr = memory->ReadByte(registers->PC);
	registers->PC++;
	registers->AF.High = memory->ReadByte(0xFF00 + addr);
}

void LD_A_FF_C::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	registers->AF.High = memory->ReadByte(0xFF00 + registers->BC.Low);
}

void DI::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	interrupts->master = 0;
}

void OR_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 n = memory->ReadByte(registers->PC);
	registers->PC++;
	CommonInstructions::Or(n, registers);
}

void LD_HL_SP_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint8_t relativeAddr = memory->ReadByte(registers->PC);
	registers->PC++;

	int32_t result = registers->SP + static_cast<int8_t>(relativeAddr);

	if (result & 0xffff0000) registers->Set(Carry);
	else registers->Clear(Carry);

	if (((registers->SP & 0x0F) + (relativeAddr & 0x0F)) > 0x0F) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);

	registers->Clear(Zero | Negative);

	registers->HL.Value = static_cast<uint16_t>(result & 0xffff);
}

void LD_SP_HL::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	registers->SP = registers->HL.Value;
}

void LD_A_NNPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u16 addr = memory->ReadWord(registers->PC);
	registers->PC += 2;

	registers->AF.High = memory->ReadByte(addr);
}

void EI::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	interrupts->master = 1;
}

void CP_N::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8 n = memory->ReadByte(registers->PC);
	registers->PC++;

	registers->Set(Negative);

	if (registers->AF.High == n) registers->Set(Zero);
	else registers->Clear(Zero);

	if (n > registers->AF.High) registers->Set(Carry);
	else registers->Clear(Carry);

	if ((n & 0x0f) > (registers->AF.High & 0x0f)) registers->Set(HalfCarry);
	else registers->Clear(HalfCarry);
}

void RLC_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::RLC(&registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::RLC(&registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::RLC(&registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::RLC(&registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::RLC(&registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::RLC(&registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::RLC(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void SWAP_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::SWAP(&registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::SWAP(&registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::SWAP(&registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::SWAP(&registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::SWAP(&registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::SWAP(&registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::SWAP(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void RES_Bit_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	u8* reg;
	switch (m_reg)
	{
	case B:
		reg = &registers->BC.High;
		break;
	case C:
		reg = &registers->BC.Low;
		break;
	case D:
		reg = &registers->DE.High;
		break;
	case E:
		reg = &registers->DE.Low;
		break;
	case H:
		reg = &registers->HL.High;
		break;
	case L:
		reg = &registers->HL.Low;
		break;
	case A:
		reg = &registers->AF.High;
		break;
	default:
		throw std::runtime_error("Invalid register");
	}

	*reg &= ~(1 << m_bit);
}

void SLA_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Sla(&registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Sla(&registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Sla(&registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Sla(&registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Sla(&registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Sla(&registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Sla(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void RR_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Rr(&registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Rr(&registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Rr(&registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Rr(&registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Rr(&registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Rr(&registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Rr(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void RL_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Rl(&registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Rl(&registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Rl(&registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Rl(&registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Rl(&registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Rl(&registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Rl(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void SRL_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Srl(&registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Srl(&registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Srl(&registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Srl(&registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Srl(&registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Srl(&registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Srl(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void BIT_N_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Bit(1 << bit, registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Bit(1 << bit, registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Bit(1 << bit, registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Bit(1 << bit, registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Bit(1 << bit, registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Bit(1 << bit, registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Bit(1 << bit, registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void BIT_N_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	CommonInstructions::Bit(1 << bit, memory->ReadByte(registers->HL.Value), registers);
}

void RES_Bit_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	auto value = memory->ReadByte(registers->HL.Value);
	value &= ~(1 << m_bit);
	memory->WriteByte(registers->HL.Value, value);
}

void SET_Bit_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	auto value = memory->ReadByte(registers->HL.Value);
	value |= 1 << m_bit;
	memory->WriteByte(registers->HL.Value, value);;
}

void SET_Bit_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		registers->BC.High |= (1 << m_bit);
		break;
	case C:
		registers->BC.Low |= (1 << m_bit);
		break;
	case D:
		registers->DE.High |= (1 << m_bit);
		break;
	case E:
		registers->DE.Low |= (1 << m_bit);
		break;
	case H:
		registers->HL.High |= (1 << m_bit);
		break;
	case L:
		registers->HL.Low |= (1 << m_bit);
		break;
	case A:
		registers->AF.High |= (1 << m_bit);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void RRC_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	uint8_t value = memory->ReadByte(registers->HL.Value);
	CommonInstructions::Rrc(&value, registers);
	memory->WriteByte(registers->HL.Value, value);
}

void SWAP_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	auto value = memory->ReadByte(registers->HL.Value);
	CommonInstructions::SWAP(&value, registers);
	memory->WriteByte(registers->HL.Value, value);
}

void SRA_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Sra(&registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Sra(&registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Sra(&registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Sra(&registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Sra(&registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Sra(&registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Sra(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void RRC_Reg8::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	switch (m_reg)
	{
	case B:
		CommonInstructions::Rrc(&registers->BC.High, registers);
		break;
	case C:
		CommonInstructions::Rrc(&registers->BC.Low, registers);
		break;
	case D:
		CommonInstructions::Rrc(&registers->DE.High, registers);
		break;
	case E:
		CommonInstructions::Rrc(&registers->DE.Low, registers);
		break;
	case H:
		CommonInstructions::Rrc(&registers->HL.High, registers);
		break;
	case L:
		CommonInstructions::Rrc(&registers->HL.Low, registers);
		break;
	case A:
		CommonInstructions::Rrc(&registers->AF.High, registers);
		break;
	default:
		throw std::runtime_error("Invalid register");
	}
}

void RLC_HLPtr::Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu)
{
	auto value = memory->ReadByte(registers->HL.Value);
	CommonInstructions::RLC(&value, registers);
	memory->WriteByte(registers->HL.Value, value);
}
