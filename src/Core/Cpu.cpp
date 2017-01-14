#include "Cpu.h"
#include "System.h"

#include <fstream>
#include <iomanip>
#include <iostream>

Cpu::Cpu(System* system) : 
	log("exec.log"), system(system)
{
	this->Halted = false;
	this->Stopped = false;
	this->Ticks = 0;
} 

Cpu::~Cpu()
{
	this->Ticks = 0;
}

void Cpu::Reset()
{
	auto registers = system->registers;
	registers->AF.High = 0x01;
	registers->AF.Low = 0xB0;
	registers->BC.High = 0x00;
	registers->BC.Low = 0x13;
	registers->DE.High = 0x00;
	registers->DE.Low = 0xD8;
	registers->HL.High = 0x01;
	registers->HL.Low = 0x4D;
	
	registers->SP = 0xfffe;
	registers->PC = 0x0100;

	this->Ticks = 0;

}

u8 Cpu::Step()
{
	if(Stopped)
	{
		return 0;
	}
#if !DEBUG
	//std::cout << std::uppercase;
	//std::cout << "PC: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->PC
	//	<< " SP: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->SP
	//	<< " AF: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->AF.Value
	//	<< " BC: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->BC.Value
	//	<< " DE: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->DE.Value
	//	<< " HL: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->HL.Value
	//	<< "\n";
	
	//log << std::uppercase;
	//log
	//	<<  "PC: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->PC
	//	<< " SP: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->SP
	//	<< " AF: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->AF.Value
	//	<< " BC: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->BC.Value
	//	<< " DE: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->DE.Value
	//	<< " HL: " << std::setfill('0') << std::setw(4) << std::hex << system->registers->HL.Value
	//	<< "\n";
#endif

	
	uint8_t instruction = system->memory->ReadByte(system->registers->PC++);

	IMemory* memory = system->memory.get();
	Registers* registers = system->registers.get();
	Interrupts* interrupts = system->interrupts.get();


	if(instruction == 0xCB)
	{
		uint8_t cbInstruction = system->memory->ReadByte(system->registers->PC++);
		cbInstructionSet[cbInstruction]->Execute(memory, registers, interrupts, this);
		this->Ticks += extendedInstructionTicks[cbInstruction];
		return extendedInstructionTicks[cbInstruction];
	}
	else
	{
		instructionSet[instruction]->Execute(memory, registers, interrupts, this);
		this->Ticks += instructionTicks[instruction];
		return instructionTicks[instruction];
	}


 }

Instruction* Cpu::instructionSet[256] = 
{ 
	new NopInstruction(),				// 0x00
	new LD_Reg16_NN(BC),				// 0x01
	new LD_Reg16Ptr_Reg8(BC,  A),		// 0x02
	new INC_Reg16(BC),					// 0x03
	new INC_Reg8(B),					// 0x04
	new DEC_Reg8(B),					// 0x05		
	new LD_Reg8_N(B),					// 0x06
	new RLCA(),							// 0x07
	new LD_NNPtr_SP(),					// 0x08
	new ADD_HL_Reg16(BC),				// 0x09
	new LD_Reg8_Reg16Ptr(A, BC),		// 0x0A
	new DEC_Reg16(BC),					// 0x0B
	new INC_Reg8(C),					// 0x0C
	new DEC_Reg8(C),					// 0x0D
	new LD_Reg8_N(C),					// 0x0E
	new RRCA(),							// 0x0F
	new STOP(),							// 0x10
	new LD_Reg16_NN(DE),				// 0x11
	new LD_Reg16Ptr_Reg8(DE, A),		// 0x12
	new INC_Reg16(DE),					// 0x13
	new INC_Reg8(D),					// 0x14
	new DEC_Reg8(D),					// 0x15
	new LD_Reg8_N(D),					// 0x16
	new RLA(),							// 0x17
	new JR_N(),							// 0x18
	new ADD_HL_Reg16(DE),				// 0x19
	new LD_Reg8_Reg16Ptr(A, DE),		// 0x1A
	new DEC_Reg16(DE),					// 0x1B
	new INC_Reg8(E),					// 0x1C
	new DEC_Reg8(E),					// 0x1D
	new LD_Reg8_N(E),					// 0x1E
	new RRA(),							// 0x1F
	new JR_NZ_N(),						// 0x20
	new LD_Reg16_NN(HL),				// 0x21
	new LDI_HLPtr_A(),			// 0x22
	new INC_Reg16(HL),					// 0x23
	new INC_Reg8(H),					// 0x24
	new DEC_Reg8(H),					// 0x25
	new LD_Reg8_N(H),					// 0x26
	new DAA(),							// 0x27
	new JR_Z_N(),						// 0x28
	new ADD_HL_Reg16(HL),				// 0x29
	new LDI_A_HLPtr(),			// 0x2A
	new DEC_Reg16(HL),					// 0x2B
	new INC_Reg8(L),					// 0x2C
	new DEC_Reg8(L),					// 0x2D
	new LD_Reg8_N(L),					// 0x2E
	new CPL(),							// 0x2F
	new JR_NC_N(),						// 0x30
	new LD_Reg16_NN(SP),				// 0x31
	new LDD_HLPtr_A(),					// 0x32
	new INC_Reg16(SP),					// 0x33
	new INC_HLPtr(),					// 0x34
	new DEC_HLPtr(),					// 0x35
	new LD_HLPtr_N(),				// 0x36
	new SCF(),							// 0x37
	new JR_C_N(),						// 0x38
	new ADD_HL_Reg16(SP),				// 0x39
	new LDD_A_HLPtr(),			// 0x3A
	new DEC_Reg16(SP),					// 0x3B
	new INC_Reg8(A),					// 0x3C
	new DEC_Reg8(A),					// 0x3D
	new LD_Reg8_N(A),					// 0x3E
	new CCF(),							// 0x3F
	new LD_Reg8_Reg8(B, B),				// 0x40
	new LD_Reg8_Reg8(B, C),				// 0x41
	new LD_Reg8_Reg8(B, D), 			// 0x42
	new LD_Reg8_Reg8(B, E),				// 0x43
	new LD_Reg8_Reg8(B, H),				// 0x44
	new LD_Reg8_Reg8(B, L),				// 0x45
	new LD_Reg8_Reg16Ptr(B, HL),		// 0x46
	new LD_Reg8_Reg8(B, A),				// 0x47
	new LD_Reg8_Reg8(C, B),				// 0x48
	new LD_Reg8_Reg8(C, C),				// 0x49
	new LD_Reg8_Reg8(C, D),				// 0x4A
	new LD_Reg8_Reg8(C, E),				// 0x4B
	new LD_Reg8_Reg8(C, H),				// 0x4C
	new LD_Reg8_Reg8(C, L),				// 0x4D
	new LD_Reg8_Reg16Ptr(C, HL),		// 0x4E
	new LD_Reg8_Reg8(C, A),				// 0x4F
	new LD_Reg8_Reg8(D, B),				// 0x50
	new LD_Reg8_Reg8(D, C),				// 0x51
	new LD_Reg8_Reg8(D, D),				// 0x52
	new LD_Reg8_Reg8(D, E),				// 0x53
	new LD_Reg8_Reg8(D, H),				// 0x54
	new LD_Reg8_Reg8(D, L),				// 0x55
	new LD_Reg8_Reg16Ptr(D, HL),		// 0x56
	new LD_Reg8_Reg8(D, A),				// 0x57
	new LD_Reg8_Reg8(E, B),				// 0x58
	new LD_Reg8_Reg8(E, C),				// 0x59
	new LD_Reg8_Reg8(E, D),				// 0x5A
	new LD_Reg8_Reg8(E, E),				// 0x5B
	new LD_Reg8_Reg8(E, H),				// 0x5C
	new LD_Reg8_Reg8(E, L),				// 0x5D
	new LD_Reg8_Reg16Ptr(E, HL),		// 0x5E
	new LD_Reg8_Reg8(E, A),				// 0x5F
	new LD_Reg8_Reg8(H, B),				// 0x60
	new LD_Reg8_Reg8(H, C),				// 0x61
	new LD_Reg8_Reg8(H, D),				// 0x62
	new LD_Reg8_Reg8(H ,E),				// 0x63
	new LD_Reg8_Reg8(H, H),				// 0x64
	new LD_Reg8_Reg8(H, L),				// 0x65
	new LD_Reg8_Reg16Ptr(H, HL),		// 0x66
	new LD_Reg8_Reg8(H, A),				// 0x67
	new LD_Reg8_Reg8(L, B),				// 0x68
	new LD_Reg8_Reg8(L, C),				// 0x69
	new LD_Reg8_Reg8(L, D),				// 0x6A
	new LD_Reg8_Reg8(L, E),				// 0x6B
	new LD_Reg8_Reg8(L, H),				// 0x6C
	new LD_Reg8_Reg8(L, L),				// 0x6D
	new LD_Reg8_Reg16Ptr(L, HL),		// 0x6E
	new LD_Reg8_Reg8(L, A),				// 0x6F
   	new LD_Reg16Ptr_Reg8(HL, B),		// 0x70
	new LD_Reg16Ptr_Reg8(HL, C),		// 0x71
	new LD_Reg16Ptr_Reg8(HL, D),		// 0x72
	new LD_Reg16Ptr_Reg8(HL, E),		// 0x73
	new LD_Reg16Ptr_Reg8(HL, H),		// 0x74
	new LD_Reg16Ptr_Reg8(HL, L),		// 0x75
	new HALT(),							// 0x76
	new LD_Reg16Ptr_Reg8(HL, A),		// 0x77
	new LD_Reg8_Reg8(A, B),				// 0x78
	new LD_Reg8_Reg8(A, C),				// 0x79
	new LD_Reg8_Reg8(A, D),				// 0x7A
	new LD_Reg8_Reg8(A, E),				// 0x7B
	new LD_Reg8_Reg8(A, H),				// 0x7C
	new LD_Reg8_Reg8(A, L),				// 0x7D
	new LD_Reg8_Reg16Ptr(A, HL),		// 0x7E
	new LD_Reg8_Reg8(A, A),				// 0x7F
	new ADD_A_Reg8(B),					// 0x80
	new ADD_A_Reg8(C),					// 0x81
	new ADD_A_Reg8(D),					// 0x82
	new ADD_A_Reg8(E),					// 0x83
	new ADD_A_Reg8(H),					// 0x84
	new ADD_A_Reg8(L),					// 0x85
	new ADD_A_HLPtr(),					// 0x86
	new ADD_A_Reg8(A),					// 0x87
	new ADC_Reg8(B),					// 0x88
	new ADC_Reg8(C),					// 0x89
	new ADC_Reg8(D),					// 0x8A
	new ADC_Reg8(E),					// 0x8B
	new ADC_Reg8(H),					// 0x8C
	new ADC_Reg8(L),					// 0x8D
	new ADC_HLPtr(),					// 0x8E
	new ADC_Reg8(A),					// 0x8F
	new SUB_Reg8(B),					// 0x90
	new SUB_Reg8(C),					// 0x91
	new SUB_Reg8(D),					// 0x92
	new SUB_Reg8(E),					// 0x93
	new SUB_Reg8(H),					// 0x94
	new SUB_Reg8(L),					// 0x95
	new SUB_HLPtr(),					// 0x96
	new SUB_Reg8(A),					// 0x97
	new SBC_Reg8(B),					// 0x98
	new SBC_Reg8(C),					// 0x99
	new SBC_Reg8(D),					// 0x9A
	new SBC_Reg8(E),					// 0x9B
	new SBC_Reg8(H),					// 0x9C
	new SBC_Reg8(L),					// 0x9D
	new SBC_HLPtr(),					// 0x9E
	new SBC_Reg8(A),					// 0x9F
	new AND_Reg8(B),					// 0xA0
	new AND_Reg8(C),					// 0xA1
	new AND_Reg8(D),					// 0xA2
	new AND_Reg8(E),					// 0xA3
	new AND_Reg8(H),					// 0xA4
	new AND_Reg8(L),					// 0xA5
	new AND_HLPtr(),					// 0xA6
	new AND_Reg8(A),					// 0xA7
	new XOR_Reg8(B),					// 0xA8
	new XOR_Reg8(C),					// 0xA9
	new XOR_Reg8(D),					// 0xAA
	new XOR_Reg8(E),					// 0xAB
	new XOR_Reg8(H),					// 0xAC
	new XOR_Reg8(L),					// 0xAD
	new XOR_HLPtr(),					// 0xAE
	new XOR_Reg8(A),					// 0xAF
	new OR_Reg8(B),						// 0xB0
	new OR_Reg8(C),						// 0xB1
	new OR_Reg8(D),						// 0xB2
	new OR_Reg8(E),						// 0xB3
	new OR_Reg8(H),						// 0xB4
	new OR_Reg8(L),						// 0xB5
	new OR_HLPtr(),					// 0xB6
	new OR_Reg8(A),						// 0xB7
	new CP_Reg8(B),						// 0xB8
	new CP_Reg8(C),						// 0xB9
	new CP_Reg8(D),						// 0xBA
	new CP_Reg8(E),						// 0xBB
	new CP_Reg8(H),						// 0xBC
	new CP_Reg8(L),						// 0xBD
	new CP_HLPtr(),					// 0xBE	
	new CP_Reg8(A),						// 0xBF
	new RET_NZ(),						// 0xC0
	new POP_Reg16(BC),					// 0xC1
	new JP_NZ_NN(),						// 0xC2 
	new JP_NN(),						// 0xC3
	new CALL_NZ_NN(),					// 0xC4
	new PUSH_Reg16(BC),					// 0xC5
	new ADD_A_N(),					// 0xC6
	new RST(0x0000),					// 0xC7
	new RET_Z(),						// 0xC8
	new RET(),							// 0xC9
	new JP_Z_NN(),						// 0xCA
	new CB_N(),							// 0xCB
	new CALL_Z_NN(),					// 0xCC
	new CALL_NN(),						// 0xCD
	new ADC_N(),						// 0xCE
	new RST(0x0008),					// 0xCF
	new RET_NC(),						// 0xD0
	new POP_Reg16(DE),					// 0xD1
	new JP_NC_NN(),						// 0xD2
	new UNKNOWN(),						// 0xD3
	new CALL_NC_NN(),					// 0xD4
	new PUSH_Reg16(DE),					// 0xD5
	new SUB_N(),						// 0xD6
	new RST(0x0010),					// 0xD7
	new RET_C(),						// 0xD8
	new RETI(),							// 0xD9
	new JP_C_NN(),						// 0xDA
	new UNKNOWN(),						// 0xDB
	new CALL_C_NN(),					// 0xDC
	new UNKNOWN(),						// 0xDD
	new SBC_N(),						// 0xDE
	new RST(0x0018),					// 0xDF
	new LD_FF_N_AP(),					// 0xE0
	new POP_Reg16(HL),					// 0xE1
	new LD_FF_C_A(),					// 0xE2
	new UNKNOWN(),						// 0xE3
	new UNKNOWN(),						// 0xE4
	new PUSH_Reg16(HL),					// 0xE5
	new AND_N(),						// 0xE6
	new RST(0x0020),					// 0xE7
	new ADD_SP_N(),					// 0xE8
	new JP_HL(),						// 0xE9
	new LD_NNPtr_A(),					// 0xEA
	new UNKNOWN(),						// 0xEB
	new UNKNOWN(),						// 0xEC
	new UNKNOWN(),						// 0xED
	new XOR_N(),						// 0xEE
	new RST(0x0028),					// 0xEF
	new LD_FF_APtr_N(),					// 0xF0
	new POP_Reg16(AF),					// 0xF1
	new LD_A_FF_C(),					// 0xF2
	new DI(),							// 0xF3
	new UNKNOWN(),						// 0xF4
	new PUSH_Reg16(AF),					// 0xF5
	new OR_N(),							// 0xF6
	new RST(0x0030),					// 0xF7
	new LD_HL_SP_N(),					// 0xF8
	new LD_SP_HL(),				// 0xF9
	new LD_A_NNPtr(),				// 0xFA
	new EI(),							// 0xFB
	new UNKNOWN(),						// 0xFC
	new UNKNOWN(),						// 0xFD
	new CP_N(),							// 0xFE
	new RST(0x0038),					// 0xFF
};


Instruction* Cpu::cbInstructionSet[256] =
{
	new RLC_Reg8(B),					// 0x00
	new RLC_Reg8(C),					// 0x01
	new RLC_Reg8(D),					// 0x02
	new RLC_Reg8(E),					// 0x03
	new RLC_Reg8(H),					// 0x04
	new RLC_Reg8(L),					// 0x05		
	new RLC_HLPtr(),					// 0x06
	new RLC_Reg8(A),					// 0x07
	new RRC_Reg8(B),					// 0x08
	new RRC_Reg8(C),					// 0x09
	new RRC_Reg8(D),					// 0x0A
	new RRC_Reg8(E),					// 0x0B
	new RRC_Reg8(H),					// 0x0C
	new RRC_Reg8(L),					// 0x0D
	new RRC_HLPtr(),					// 0x0E
	new RRC_Reg8(A),					// 0x0F
	new RL_Reg8(B),						// 0x10
	new RL_Reg8(C),						// 0x11
	new RL_Reg8(D),						// 0x12
	new RL_Reg8(E),						// 0x13
	new RL_Reg8(H),						// 0x14
	new RL_Reg8(L),						// 0x15
	new NotImplementedInstruction(),	// 0x16
	new RL_Reg8(A),						// 0x17
	new RR_Reg8(B),						// 0x18
	new RR_Reg8(C),						// 0x19
	new RR_Reg8(D),						// 0x1A
	new RR_Reg8(E),						// 0x1B
	new RR_Reg8(H),						// 0x1C
	new RR_Reg8(L),						// 0x1D
	new NotImplementedInstruction(),	// 0x1E
	new RR_Reg8(A),						// 0x1F
	new SLA_Reg8(B),					// 0x20
	new SLA_Reg8(C),					// 0x21
	new SLA_Reg8(D),					// 0x22
	new SLA_Reg8(E),					// 0x23
	new SLA_Reg8(H),					// 0x24
	new SLA_Reg8(L),					// 0x25
	new NotImplementedInstruction(),	// 0x26
	new SLA_Reg8(A),					// 0x27
	new SRA_Reg8(B),					// 0x28
	new SRA_Reg8(C),					// 0x29
	new SRA_Reg8(D),					// 0x2A
	new SRA_Reg8(E),					// 0x2B
	new SRA_Reg8(H),					// 0x2C
	new SRA_Reg8(L),					// 0x2D
	new NotImplementedInstruction(),	// 0x2E
	new SRA_Reg8(A),					// 0x2F
	new SWAP_Reg8(B),					// 0x30
	new SWAP_Reg8(C),					// 0x31
	new SWAP_Reg8(D),					// 0x32
	new SWAP_Reg8(E),					// 0x33
	new SWAP_Reg8(H),					// 0x34
	new SWAP_Reg8(L),					// 0x35
	new SWAP_HLPtr(),					// 0x36
	new SWAP_Reg8(A),					// 0x37
	new SRL_Reg8(B),					// 0x38
	new SRL_Reg8(C),					// 0x39
	new SRL_Reg8(D),					// 0x3A
	new SRL_Reg8(E),					// 0x3B
	new SRL_Reg8(H),					// 0x3C
	new SRL_Reg8(L),					// 0x3D
	new NotImplementedInstruction(),	// 0x3E
	new SRL_Reg8(A),					// 0x3F
	new BIT_N_Reg8(0, B),				// 0x40
	new BIT_N_Reg8(0, C),				// 0x41
	new BIT_N_Reg8(0, D), 				// 0x42
	new BIT_N_Reg8(0, E),				// 0x43
	new BIT_N_Reg8(0, H),				// 0x44
	new BIT_N_Reg8(0, L),				// 0x45
	new BIT_N_HLPtr(0),					// 0x46
	new BIT_N_Reg8(0, A),				// 0x47
	new BIT_N_Reg8(1, B),				// 0x48
	new BIT_N_Reg8(1, C),				// 0x49
	new BIT_N_Reg8(1, D),				// 0x4A
	new BIT_N_Reg8(1, E),				// 0x4B
	new BIT_N_Reg8(1, H),				// 0x4C
	new BIT_N_Reg8(1, L),				// 0x4D
	new BIT_N_HLPtr(1),					// 0x4E
	new BIT_N_Reg8(1, A),				// 0x4F
	new BIT_N_Reg8(2, B),				// 0x50
	new BIT_N_Reg8(2, C),				// 0x51
	new BIT_N_Reg8(2, D),				// 0x52
	new BIT_N_Reg8(2, E),				// 0x53
	new BIT_N_Reg8(2, H),				// 0x54
	new BIT_N_Reg8(2, L),				// 0x55
	new BIT_N_HLPtr(2),					// 0x56
	new BIT_N_Reg8(2, A),				// 0x57
	new BIT_N_Reg8(3, B),				// 0x58
	new BIT_N_Reg8(3, C),				// 0x59
	new BIT_N_Reg8(3, D),				// 0x5A
	new BIT_N_Reg8(3, E),				// 0x5B
	new BIT_N_Reg8(3, H),				// 0x5C
	new BIT_N_Reg8(3, L),				// 0x5D
	new BIT_N_HLPtr(3),					// 0x5E
	new BIT_N_Reg8(3, A),				// 0x5F
	new BIT_N_Reg8(4, B),				// 0x60
	new BIT_N_Reg8(4, C),				// 0x61
	new BIT_N_Reg8(4, D),				// 0x62
	new BIT_N_Reg8(4, E),				// 0x63
	new BIT_N_Reg8(4, H),				// 0x64
	new BIT_N_Reg8(4, L),				// 0x65
	new BIT_N_HLPtr(4),					// 0x66
	new BIT_N_Reg8(4, A),				// 0x67
	new BIT_N_Reg8(5, B),				// 0x68
	new BIT_N_Reg8(5, C),				// 0x69
	new BIT_N_Reg8(5, D),				// 0x6A
	new BIT_N_Reg8(5, E),				// 0x6B
	new BIT_N_Reg8(5, H),				// 0x6C
	new BIT_N_Reg8(5, L),				// 0x6D
	new BIT_N_HLPtr(5),					// 0x6E
	new BIT_N_Reg8(5, A),				// 0x6F
	new BIT_N_Reg8(6, B),				// 0x70
	new BIT_N_Reg8(6, C),				// 0x71
	new BIT_N_Reg8(6, D),				// 0x72
	new BIT_N_Reg8(6, E),				// 0x73
	new BIT_N_Reg8(6, H),				// 0x74
	new BIT_N_Reg8(6, L),				// 0x75
	new BIT_N_HLPtr(6),					// 0x76
	new BIT_N_Reg8(6, A),				// 0x77
	new BIT_N_Reg8(7, B),				// 0x78
	new BIT_N_Reg8(7, C),				// 0x79
	new BIT_N_Reg8(7, D),				// 0x7A
	new BIT_N_Reg8(7, E),				// 0x7B
	new BIT_N_Reg8(7, H),				// 0x7C
	new BIT_N_Reg8(7, L),				// 0x7D
	new BIT_N_HLPtr(7),					// 0x7E
	new BIT_N_Reg8(7, A),				// 0x7F
	new RES_Bit_Reg8(0, B),				// 0x80
	new RES_Bit_Reg8(0, C),				// 0x81
	new RES_Bit_Reg8(0, D),				// 0x82
	new RES_Bit_Reg8(0, E),				// 0x83
	new RES_Bit_Reg8(0, H),				// 0x84
	new RES_Bit_Reg8(0, L),				// 0x85
	new RES_Bit_HLPtr(0),				// 0x86
	new RES_Bit_Reg8(0, A),				// 0x87
	new RES_Bit_Reg8(1, B),				// 0x88
	new RES_Bit_Reg8(1, C),				// 0x89
	new RES_Bit_Reg8(1, D),				// 0x8A
	new RES_Bit_Reg8(1, E),				// 0x8B
	new RES_Bit_Reg8(1, H),				// 0x8C
	new RES_Bit_Reg8(1, L),				// 0x8D
	new RES_Bit_HLPtr(1),				// 0x8E
	new RES_Bit_Reg8(1, A),				// 0x8F
	new RES_Bit_Reg8(2, B),				// 0x90
	new RES_Bit_Reg8(2, C),				// 0x91
	new RES_Bit_Reg8(2, D),				// 0x92
	new RES_Bit_Reg8(2, E),				// 0x93
	new RES_Bit_Reg8(2, H),				// 0x94
	new RES_Bit_Reg8(2, L),				// 0x95
	new RES_Bit_HLPtr(3),				// 0x96
	new RES_Bit_Reg8(2, A),				// 0x97
	new RES_Bit_Reg8(3, B),				// 0x98
	new RES_Bit_Reg8(3, C),				// 0x99
	new RES_Bit_Reg8(3, D),				// 0x9A
	new RES_Bit_Reg8(3, E),				// 0x9B
	new RES_Bit_Reg8(3, H),				// 0x9C
	new RES_Bit_Reg8(3, L),				// 0x9D
	new RES_Bit_HLPtr(3),				// 0x9E
	new RES_Bit_Reg8(3, A),				// 0x9F
	new RES_Bit_Reg8(4, B),				// 0xA0
	new RES_Bit_Reg8(4, C),				// 0xA1
	new RES_Bit_Reg8(4, D),				// 0xA2
	new RES_Bit_Reg8(4, E),				// 0xA3
	new RES_Bit_Reg8(4, H),				// 0xA4
	new RES_Bit_Reg8(4, L),				// 0xA5
	new RES_Bit_HLPtr(4),				// 0xA6
	new RES_Bit_Reg8(4, A),				// 0xA7
	new RES_Bit_Reg8(5, B),				// 0xA8
	new RES_Bit_Reg8(5, C),				// 0xA9
	new RES_Bit_Reg8(5, D),				// 0xAA
	new RES_Bit_Reg8(5, E),				// 0xAB
	new RES_Bit_Reg8(5, H),				// 0xAC
	new RES_Bit_Reg8(5, L),				// 0xAD
	new RES_Bit_HLPtr(5),				// 0xAE
	new RES_Bit_Reg8(5, A),				// 0xAF
	new RES_Bit_Reg8(6, B),				// 0xB0
	new RES_Bit_Reg8(6, C),				// 0xB1
	new RES_Bit_Reg8(6, D),				// 0xB2
	new RES_Bit_Reg8(6, E),				// 0xB3
	new RES_Bit_Reg8(6, H),				// 0xB4
	new RES_Bit_Reg8(6, L),				// 0xB5
	new RES_Bit_HLPtr(6),				// 0xB6
	new RES_Bit_Reg8(6, A),				// 0xB7
	new RES_Bit_Reg8(7, B),				// 0xB8
	new RES_Bit_Reg8(7, C),				// 0xB9
	new RES_Bit_Reg8(7, D),				// 0xBA
	new RES_Bit_Reg8(7, E),				// 0xBB
	new RES_Bit_Reg8(7, H),				// 0xBC
	new RES_Bit_Reg8(7, L),				// 0xBD
	new RES_Bit_HLPtr(7),				// 0xBE	
	new RES_Bit_Reg8(7, A),				// 0xBF
	new SET_Bit_Reg8(0, B),				// 0xC0
	new SET_Bit_Reg8(0, C),				// 0xC1
	new SET_Bit_Reg8(0, D),				// 0xC2
	new SET_Bit_Reg8(0, E),				// 0xC3
	new SET_Bit_Reg8(0, H),				// 0xC4
	new SET_Bit_Reg8(0, L),				// 0xC5
	new SET_Bit_HLPtr(0),				// 0xC6
	new SET_Bit_Reg8(0, A),				// 0xC7
	new SET_Bit_Reg8(1, B),				// 0xC8
	new SET_Bit_Reg8(1, C),				// 0xC9
	new SET_Bit_Reg8(1, D),				// 0xCA
	new SET_Bit_Reg8(1, E),				// 0xCB
	new SET_Bit_Reg8(1, H),				// 0xCC
	new SET_Bit_Reg8(1, L),				// 0xCD
	new SET_Bit_HLPtr(1),				// 0xCE
	new SET_Bit_Reg8(1, A),				// 0xCF
	new SET_Bit_Reg8(2, B),				// 0xD0
	new SET_Bit_Reg8(2, C),				// 0xD1
	new SET_Bit_Reg8(2, D),				// 0xD2
	new SET_Bit_Reg8(2, E),				// 0xD3
	new SET_Bit_Reg8(2, H),				// 0xD4
	new SET_Bit_Reg8(2, L),				// 0xD5
	new SET_Bit_HLPtr(2),				// 0xD6
	new SET_Bit_Reg8(2, A),				// 0xD7
	new SET_Bit_Reg8(3, B),				// 0xD8
	new SET_Bit_Reg8(3, C),				// 0xD9
	new SET_Bit_Reg8(3, D),				// 0xDA
	new SET_Bit_Reg8(3, E),				// 0xDB
	new SET_Bit_Reg8(3, H),				// 0xDC
	new SET_Bit_Reg8(3, L),				// 0xDD
	new SET_Bit_HLPtr(3),				// 0xDE
	new SET_Bit_Reg8(3, A),				// 0xDF
	new SET_Bit_Reg8(4, B),				// 0xE0
	new SET_Bit_Reg8(4, C),				// 0xE1
	new SET_Bit_Reg8(4, D),				// 0xE2
	new SET_Bit_Reg8(4, E),				// 0xE3
	new SET_Bit_Reg8(4, H),				// 0xE4
	new SET_Bit_Reg8(4, L),				// 0xE5
	new SET_Bit_HLPtr(4),				// 0xE6
	new SET_Bit_Reg8(4, A),				// 0xE7
	new SET_Bit_Reg8(5, B),				// 0xE8
	new SET_Bit_Reg8(5, C),				// 0xE9
	new SET_Bit_Reg8(5, D),				// 0xEA
	new SET_Bit_Reg8(5, E),				// 0xEB
	new SET_Bit_Reg8(5, H),				// 0xEC
	new SET_Bit_Reg8(5, L),				// 0xED
	new SET_Bit_HLPtr(5),				// 0xEE
	new SET_Bit_Reg8(5, A),				// 0xEF
	new SET_Bit_Reg8(6, B),				// 0xF0
	new SET_Bit_Reg8(6, C),				// 0xF1
	new SET_Bit_Reg8(6, D),				// 0xF2
	new SET_Bit_Reg8(6, E),				// 0xF3
	new SET_Bit_Reg8(6, H),				// 0xF4
	new SET_Bit_Reg8(6, L),				// 0xF5
	new SET_Bit_HLPtr(6),				// 0xF6
	new SET_Bit_Reg8(6, A),				// 0xF7
	new SET_Bit_Reg8(7, B),				// 0xF8
	new SET_Bit_Reg8(7, C),				// 0xF9
	new SET_Bit_Reg8(7, D),				// 0xFA
	new SET_Bit_Reg8(7, E),				// 0xFB
	new SET_Bit_Reg8(7, H),				// 0xFC
	new SET_Bit_Reg8(7, L),				// 0xFD
	new SET_Bit_HLPtr(7),				// 0xFE
	new SET_Bit_Reg8(7, A),				// 0xFF
};