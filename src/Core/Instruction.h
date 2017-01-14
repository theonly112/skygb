#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <sstream>

#include "Registers.h"
#include "Memory.h"
#include "Interrupts.h"

class IMemory;
class Cpu;

class Instruction
{
protected:
	virtual ~Instruction() {}
public:
	virtual void Execute(IMemory * memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) = 0;
};

class CommonInstructions
{
public:
	static void Xor(u8 value, Registers* registers);
	static void PushU16(u16 value, Registers* registers, IMemory* memory);
	static void Sub(u8 value, Registers* registers);
	static u16 PopU16(Registers* registers, IMemory* memory);
	static void RLC(u8* reg8, Registers* registers);
	static void Sbc(u8 high, Registers* registers);
	static void Or(u8 high, Registers* registers);
	static void Add(u8* destination, u8 value, Registers* registers);
	static void Cp(u8 high, Registers* registers);
	static void SWAP(u8* high, Registers* registers);
	static void And(u8 low, Registers* registers);
	static void Inc(u8* reg, Registers* registers);
	static void Sla(u8* high, Registers* registers);
	static void Adc(uint8_t uint8_t, Registers* registers);
	static void Rr(u8* high, Registers* registers);
	static void Rl(u8* low, Registers* registers);
	static void Srl(u8* high, Registers* registers);
	static void Dec(u8* hl_value, Registers* registers);
	static void Bit(u8 bit, u8 value, Registers* registers);
	static void Rrc(uint8_t* value, Registers* registers);
	static void Sra(u8* high, Registers* registers);
};

class NopInstruction : public Instruction
{
	void Execute(IMemory * memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override
	{
		// TODO: implement nop
	}
};


class NotImplementedInstruction : public Instruction
{
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override
	{
		//std::stringstream fmt;
		//auto instruction = memory->ReadByte(registers->PC - 1);
		//fmt << "Instruction " << static_cast<int>(instruction) << " is not implemented";
		//throw std::runtime_error(fmt.str().c_str());
		throw std::runtime_error("Instruction not implemented");
	}
};

class LD_Reg16_NN : public Instruction
{
	RegisterNames m_reg;
public:
	LD_Reg16_NN(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(SixteenBitRegister* reg, u16 val);
	static void Execute(u16* reg, u16 val);
};

class LD_Reg16Ptr_Reg8 : public Instruction
{
	RegisterNames m_reg16;
	RegisterNames m_reg8;
public:
	LD_Reg16Ptr_Reg8(RegisterNames reg16, RegisterNames reg8) : m_reg16(reg16), m_reg8(reg8) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(IMemory* memory, SixteenBitRegister* reg16, u8 reg8Value);
};

class INC_Reg16 : public Instruction
{
	RegisterNames m_reg;
public:
	INC_Reg16(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(SixteenBitRegister* reg);
	static void Execute(u16* reg);
};

class INC_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	INC_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(u8* reg, Registers* registers);
};

class DEC_Reg16 : public Instruction
{
	RegisterNames m_reg;
public:
	DEC_Reg16(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(u16* reg);
};

class DEC_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	DEC_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(u8* reg, Registers* regs);
};

class LD_Reg8_N : public Instruction
{
	RegisterNames m_reg;
public:
	LD_Reg8_N(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(u8* reg, u8 value);
};

class RLCA : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_NNPtr_SP : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class ADD_HL_Reg16 : public Instruction
{
	RegisterNames m_reg;
public:
	ADD_HL_Reg16(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(SixteenBitRegister* regA, SixteenBitRegister* regB, Registers* registers);
	static void Execute(SixteenBitRegister* regA, u16 regBValue, Registers* registers);
};


class LD_Reg8_Reg16Ptr : public Instruction
{
	RegisterNames m_reg8;
	RegisterNames m_reg16;
public:
	LD_Reg8_Reg16Ptr(RegisterNames reg8, RegisterNames reg16) : m_reg8(reg8), m_reg16(reg16){}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
	static void Execute(IMemory* memory, u8* regA, SixteenBitRegister* regB);
};

class RRCA : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class STOP : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RLA : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JR_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RRA : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JR_NZ_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LDI_HLPtr_A : public Instruction 
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class DAA : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JR_Z_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JR_NC_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LDD_HLPtr_A : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class INC_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class DEC_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_HLPtr_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SCF : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JR_C_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_Reg8_Reg8 : public Instruction
{
	RegisterNames m_regL;
	RegisterNames m_regR;
public:
	LD_Reg8_Reg8(RegisterNames regL, RegisterNames regR) : m_regL(regL), m_regR(regR) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LDI_A_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CPL : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LDD_A_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CCF : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class HALT : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class ADD_A_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	ADD_A_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class ADD_A_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class ADC_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	ADC_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class ADC_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SUB_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	SUB_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SUB_HLPtr : public Instruction 
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SBC_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	SBC_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SBC_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class AND_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	AND_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class AND_HLPtr : public Instruction 
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class XOR_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	XOR_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class XOR_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class OR_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	OR_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class OR_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CP_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	CP_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CP_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RET_NZ : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class POP_Reg16 : public Instruction
{
	RegisterNames m_reg;
public:
	POP_Reg16(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JP_NZ_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JP_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CALL_NZ_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class PUSH_Reg16 : public Instruction
{
	RegisterNames m_reg;
public:
	PUSH_Reg16(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class ADD_A_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RST : public Instruction
{
	u16 rst_value;
public:
	RST(u16 rst) : rst_value(rst) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RET_Z : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RET : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JP_Z_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CB_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CALL_Z_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CALL_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class ADC_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RET_NC : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JP_NC_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class UNKNOWN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CALL_NC_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SUB_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RET_C : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RETI : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JP_C_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CALL_C_NN : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SBC_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_FF_N_AP : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_FF_C_A :  public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class AND_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class ADD_SP_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class JP_HL : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_NNPtr_A : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class XOR_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_FF_APtr_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_A_FF_C : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class DI : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class OR_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_HL_SP_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_SP_HL : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class LD_A_NNPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class EI : public Instruction 
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class CP_N : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};


// Start CB Instructions

class RLC_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	RLC_Reg8(RegisterNames reg) : m_reg(reg){}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SWAP_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	SWAP_Reg8(RegisterNames reg) : m_reg(reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RES_Bit_Reg8 : public Instruction
{
	u8 m_bit;
	RegisterNames m_reg;
public:
	RES_Bit_Reg8(u8 bit, RegisterNames reg) : m_bit(bit), m_reg(reg){}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SLA_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	SLA_Reg8(RegisterNames m_reg) : m_reg(m_reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RR_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	RR_Reg8(RegisterNames m_reg) : m_reg(m_reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RL_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	RL_Reg8(RegisterNames m_reg) : m_reg(m_reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SRL_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	SRL_Reg8(RegisterNames m_reg) : m_reg(m_reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class BIT_N_Reg8 : public Instruction
{
	RegisterNames m_reg;
	u8 bit;
public:
	BIT_N_Reg8(u8 bit, RegisterNames m_reg) : m_reg(m_reg), bit(bit) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class BIT_N_HLPtr : public Instruction
{
	u8 bit;
public:
	BIT_N_HLPtr(u8 bit) : bit(bit) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RES_Bit_HLPtr : public Instruction
{
	u8 m_bit;
public:
	RES_Bit_HLPtr(u8 bit) : m_bit(bit) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SET_Bit_HLPtr : public Instruction
{
	u8 m_bit;
public:
	SET_Bit_HLPtr(u8 bit) : m_bit(bit) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SET_Bit_Reg8 : public Instruction
{
	u8 m_bit;
	RegisterNames m_reg;
public:
	SET_Bit_Reg8(u8 bit, RegisterNames m_reg) : m_reg(m_reg), m_bit(bit) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RRC_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SWAP_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class SRA_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	SRA_Reg8(RegisterNames m_reg) : m_reg(m_reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RRC_Reg8 : public Instruction
{
	RegisterNames m_reg;
public:
	RRC_Reg8(RegisterNames m_reg) : m_reg(m_reg) {}
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};

class RLC_HLPtr : public Instruction
{
public:
	void Execute(IMemory* memory, Registers* registers, Interrupts* interrupts, Cpu* cpu) override;
};
#endif
