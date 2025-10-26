#include "Instruction.h"

#include "Memory.h"

static uint8_t InstructionDecodeAddressingInput(CPU* cpu) {
	switch (DecodeInstructionB(cpu->currentOpCode)) {
		case ADDRESSING_MODE_IMMEDIATE: return *((uint8_t*)cpu->arg);
		case ADDRESSING_MODE_ZEROPAGE: return MEMORY_GET_BYTE(cpu->RAM, *((uint8_t*)cpu->arg));
		case ADDRESSING_MODE_ZEROPAGE_INDEXED_X: return MEMORY_GET_BYTE(cpu->RAM, *((uint8_t*)(cpu->arg)) + cpu->X);
		case ADDRESSING_MODE_ABSOLUTE: return MEMORY_GET_BYTE(cpu->RAM, *((uint16_t*)(cpu->arg)));
		case ADDRESSING_MODE_ABSOLUTE_INDEXED_X: return MEMORY_GET_BYTE(cpu->RAM, *((uint16_t*)(cpu->arg)) + cpu->X);
		case ADDRESSING_MODE_ABSOLUTE_INDEXED_Y: return MEMORY_GET_BYTE(cpu->RAM, *((uint16_t*)(cpu->arg)) + cpu->Y);
		case ADDRESSING_MODE_INDEXED_X_INDIRECT: return MEMORY_GET_BYTE(cpu->RAM, (uint8_t)(*((uint8_t*)cpu->arg)) + cpu->X);  // works
		case ADDRESSING_MODE_INDIRECT_INDEXED_Y:
			return MEMORY_GET_BYTE(cpu->RAM, MEMORY_GET_WORD(cpu->RAM, *((uint8_t*)(cpu->arg))) + cpu->Y);	// works
	}
	return 0;
}

static uint16_t InstructionDecodeAddressingDest(CPU* cpu) {
	switch (DecodeInstructionB(cpu->currentOpCode)) {
		case ADDRESSING_MODE_ZEROPAGE: return *((uint8_t*)cpu->arg);
		case ADDRESSING_MODE_ZEROPAGE_INDEXED_X: return *((uint8_t*)(cpu->arg)) + cpu->X;
		case ADDRESSING_MODE_ABSOLUTE: return *((uint16_t*)(cpu->arg));
		case ADDRESSING_MODE_ABSOLUTE_INDEXED_X: return *((uint16_t*)(cpu->arg)) + cpu->X;
		case ADDRESSING_MODE_ABSOLUTE_INDEXED_Y: return *((uint16_t*)(cpu->arg)) + cpu->Y;
		case ADDRESSING_MODE_INDEXED_X_INDIRECT: return MEMORY_GET_WORD(cpu->RAM, (uint8_t)(*((uint8_t*)cpu->arg)) + cpu->X);
		case ADDRESSING_MODE_INDIRECT_INDEXED_Y: return MEMORY_GET_WORD(cpu->RAM, *((uint8_t*)(cpu->arg))) + cpu->Y;
		default: return 0;
	}
}

static inline void PushByte(CPU* cpu, uint8_t data) {
	MEMORY_SET_BYTE(cpu->RAM, cpu->S + 0x100, data);
	cpu->S--;
}

static inline void PushWord(CPU* cpu, uint16_t data) {
	MEMORY_SET_BYTE(cpu->RAM, cpu->S + 0x100, HIGHBYTE(data));
	cpu->S--;
	MEMORY_SET_BYTE(cpu->RAM, cpu->S + 0x100, LOWBYTE(data));
	cpu->S--;
}

static inline uint8_t PopByte(CPU* cpu) {
	cpu->S++;
	return MEMORY_GET_BYTE(cpu->RAM, cpu->S + 0x100);
}

static inline uint16_t PopWord(CPU* cpu) {
	cpu->S++;
	uint8_t low = MEMORY_GET_BYTE(cpu->RAM, cpu->S + 0x100);
	cpu->S++;
	uint8_t high = MEMORY_GET_BYTE(cpu->RAM, cpu->S + 0x100);
	return WORDCAT(high, low);
}

void InstructionNOP(CPU* cpu, uint8_t pad) {  // 0x02, 0x03, 0x0B, 0x13, 0x1B, 0x22, 0x23, 0x2B, 0x33, 0x3B, 0x42, 0x43, 0x44, 0x4B, 0x53, 0x54,
											  // 0x5B, 0x5C, 0x62, 0x63, 0x6B, 0x73, 0x7B,
	// 0x82, 0x83, 0x8B, 0x93, 0x9B, 0xA3, 0xAB, 0xB3, 0xBB, 0xC2, 0xC3, 0xD3, 0xD4, 0xDC, 0xE2, 0xE3, 0xEA, 0xEB, 0xF3, 0xF4, 0xFB, 0xFC
}

void InstructionBRK(CPU* cpu, uint8_t pad) {  // 0x00
}

void InstructionBIT(CPU* cpu, uint8_t pad) {  // 0x24, 0x2C, 0x34, 0x3C, 0x89
	uint8_t val;
	uint8_t tmp;
	switch (cpu->currentOpCode) {
		case INSTRUCTION_SPECIAL_BIT_IMM_A: val = *((uint8_t*)cpu->arg); break;
		case INSTRUCTION_SPECIAL_BIT_ZPG_X_A: val = MEMORY_GET_BYTE(cpu->RAM, *((uint8_t*)(cpu->arg + cpu->X))); break;
		case INSTRUCTION_SPECIAL_BIT_ABS_X_A: val = MEMORY_GET_BYTE(cpu->RAM, *((uint16_t*)(cpu->arg + cpu->X))); break;
		default: val = InstructionDecodeAddressingInput(cpu);
	}
	tmp = val & cpu->A;
	cpu->F.flags.N = tmp & 0x80;
	cpu->F.flags.V = tmp & 1;
	cpu->F.flags.Z = tmp == 0;
}
void InstructionTSB(CPU* cpu, uint8_t pad) {  // 0x04, 0x0C
	uint16_t address;
	uint8_t data;
	uint8_t tmp;
	switch (cpu->currentOpCode) {
		case INSTRUCTION_SPECIAL_TSB_ZPG: address = *((uint8_t*)cpu->arg); break;
		case INSTRUCTION_SPECIAL_TSB_ABS: address = *((uint16_t*)(cpu->arg)); break;
		default: address = 0;
	}
	data = MEMORY_GET_BYTE(cpu->RAM, address);
	tmp = data & cpu->A;
	data = data & ~(cpu->A);
	MEMORY_SET_BYTE(cpu->RAM, address, data);
	cpu->F.flags.Z = tmp == 0;
}
void InstructionTRB(CPU* cpu, uint8_t pad) {  // 0x14, 0x1C
	uint16_t address;
	uint8_t data;
	uint8_t tmp;
	switch (cpu->currentOpCode) {
		case INSTRUCTION_SPECIAL_TRB_ZPG: address = *((uint8_t*)cpu->arg); break;
		case INSTRUCTION_SPECIAL_TRB_ABS: address = *((uint16_t*)(cpu->arg)); break;
		default: address = 0;
	}
	data = MEMORY_GET_BYTE(cpu->RAM, address);
	tmp = data & cpu->A;
	data = data | cpu->A;
	MEMORY_SET_BYTE(cpu->RAM, address, data);
	cpu->F.flags.Z = tmp != 0;
}

void InstructionASL(CPU* cpu, uint8_t pad) {  // 0x06, 0x0A, 0x0E, 0x16, 0x1E
	uint16_t dest;
	uint8_t data;
	uint16_t tmp;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_ASL_A) {	// works
		data = cpu->A;
		tmp = data << 1;
		cpu->A = tmp;
	} else {
		dest = InstructionDecodeAddressingDest(cpu);
		data = MEMORY_GET_BYTE(cpu->RAM, dest);
		tmp = data << 1;
		MEMORY_SET_BYTE(cpu->RAM, dest, tmp);
	}
	cpu->F.flags.C = (data & 0x80);
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
}
void InstructionLSR(CPU* cpu, uint8_t pad) {  // 0x46, 0x4A, 0x4E, 0x56, 0x5E
	uint16_t dest;
	uint8_t data;
	uint16_t tmp;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_LSR_A) {	// works
		data = cpu->A;
		tmp = data >> 1;
		cpu->A = tmp;
	} else {
		dest = InstructionDecodeAddressingDest(cpu);
		data = MEMORY_GET_BYTE(cpu->RAM, dest);
		tmp = data >> 1;
		MEMORY_SET_BYTE(cpu->RAM, dest, tmp);
	}
	cpu->F.flags.C = (data & 1);
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
}
void InstructionROL(CPU* cpu, uint8_t pad) {
	uint16_t dest;
	uint8_t data;
	uint16_t tmp;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_ROL_A) {	// works
		data = cpu->A;
		tmp = data << 1;
		tmp |= cpu->F.flags.C;
		cpu->A = tmp;
	} else {
		dest = InstructionDecodeAddressingDest(cpu);
		data = MEMORY_GET_BYTE(cpu->RAM, dest);
		tmp = data << 1;
		tmp |= cpu->F.flags.C;
		MEMORY_SET_BYTE(cpu->RAM, dest, tmp);
	}
	cpu->F.flags.C = (data & 0x80);
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
}  // 0x26, 0x2A, 0x2E, 0x36, 0x3E
void InstructionROR(CPU* cpu, uint8_t pad) {  // 0x66, 0x6A, 0x6E, 0x76, 0x7E
	uint16_t dest;
	uint8_t data;
	uint16_t tmp;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_ROR_A) {	// works
		data = cpu->A;
		tmp = data >> 1;
		tmp |= cpu->F.flags.C << 7;
		cpu->A = tmp;
	} else {
		dest = InstructionDecodeAddressingDest(cpu);
		data = MEMORY_GET_BYTE(cpu->RAM, dest);
		tmp = data >> 1;
		tmp |= cpu->F.flags.C << 7;
		MEMORY_SET_BYTE(cpu->RAM, dest, tmp);
	}
	cpu->F.flags.C = (data & 1);
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
}

// works
void InstructionSMB(CPU* cpu, uint8_t bitPos) {	 // 0x87, 0x97, 0xA7, 0xB7, 0xC7, 0xD7, 0xE7, 0xF7
	uint8_t address;
	address = *((uint8_t*)cpu->arg);
	MEMORY_SET_BYTE(cpu->RAM, address, MEMORY_GET_BYTE(cpu->RAM, address) | (1 << bitPos));
}
void InstructionRMB(CPU* cpu, uint8_t bitPos) {	 // 0x07, 0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77
	uint8_t address;
	address = *((uint8_t*)cpu->arg);
	MEMORY_SET_BYTE(cpu->RAM, address, MEMORY_GET_BYTE(cpu->RAM, address) & ~(1 << bitPos));
}

// works (including Decimal mode as far as i know)
void InstructionADC(CPU* cpu, uint8_t pad) {  // 0x61, 0x65, 0x69, 0x6D, 0x71, 0x72, 0x75, 0x79, 0x7D
	uint8_t val;
	uint16_t tmp;
	val = InstructionDecodeAddressingInput(cpu);

	if (cpu->F.flags.C) tmp++;
	if (cpu->F.flags.D) {
		uint8_t al;
		al = ((cpu->A & 0xF) + (val & 0xF)) + cpu->F.flags.C;
		if (al >= 0x0A) {
			al = ((al + 0x06) & 0x0F) + 0x10;
		}
		tmp = (cpu->A & 0xF0) + (val & 0xF0) + al;
		if (tmp >= 0xA0) {
			tmp += 0x60;
		}
		cpu->F.flags.C = tmp >= 0x100;
	} else {
		tmp = cpu->A + val + cpu->F.flags.C;
		cpu->F.flags.C = tmp >= 0x100;
	}
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
	cpu->F.flags.N = tmp & 0x80;
	cpu->F.flags.V = ((cpu->A & 0x80) == (val & 0x80)) && ((val & 0x80) != (tmp & 0x80));
	cpu->A = tmp;
}
#include <stdio.h>
void InstructionSBC(CPU* cpu, uint8_t pad) {  // 0xE1, 0xE5, 0xE9, 0xED, 0xF1, 0xF2, 0xF5, 0xF9, 0xFD
	uint8_t val;
	int16_t tmp;
	val = InstructionDecodeAddressingInput(cpu);
	tmp = cpu->A - val;
	if (!cpu->F.flags.C) tmp--;
	if (cpu->F.flags.D) {
		int16_t al;
		al = ((cpu->A & 0xF) - (val & 0xF)) + cpu->F.flags.C - 1;
		if (al < 0) {
			al = ((al - 0x06) & 0x0F) - 0x10;
		}
		tmp = (cpu->A & 0xF0) - (val & 0xF0) + al;
		if (tmp < 0) {
			tmp -= 0x60;
		}
		cpu->F.flags.C = !(tmp < 0);
	} else {
		tmp = cpu->A - val + cpu->F.flags.C - 1;
		cpu->F.flags.C = !(tmp < 0);
	}
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
	cpu->F.flags.N = tmp & 0x80;
	cpu->F.flags.V = ((cpu->A & 0x80) != (val & 0x80)) && ((cpu->A & 0x80) != (tmp & 0x80));
	cpu->A = (uint8_t)tmp;
}

void InstructionTAX(CPU* cpu, uint8_t pad) {  // 0xAA
	cpu->X = cpu->A;
	cpu->F.flags.Z = cpu->X == 0;
	cpu->F.flags.N = (cpu->X & 0x80) == 0x80;
}
void InstructionTAY(CPU* cpu, uint8_t pad) {  // 0xA8
	cpu->Y = cpu->A;
	cpu->F.flags.Z = cpu->Y == 0;
	cpu->F.flags.N = (cpu->Y & 0x80) == 0x80;
}
void InstructionTXA(CPU* cpu, uint8_t pad) {  // 0x8A
	cpu->A = cpu->X;
	cpu->F.flags.Z = cpu->A == 0;
	cpu->F.flags.N = (cpu->A & 0x80) == 0x80;
}
void InstructionTYA(CPU* cpu, uint8_t pad) {  // 0x98
	cpu->A = cpu->Y;
	cpu->F.flags.Z = cpu->A == 0;
	cpu->F.flags.N = (cpu->A & 0x80) == 0x80;
}
void InstructionTXS(CPU* cpu, uint8_t pad) {  // 0x9A
	cpu->S = cpu->X;
	cpu->F.flags.Z = cpu->S == 0;
	cpu->F.flags.N = (cpu->S & 0x80) == 0x80;
}
void InstructionTSX(CPU* cpu, uint8_t pad) {  // 0xBA
	cpu->X = cpu->S;
	cpu->F.flags.Z = cpu->X == 0;
	cpu->F.flags.N = (cpu->X & 0x80) == 0x80;
}

void InstructionLDA(CPU* cpu, uint8_t pad) {  // 0xA1, 0xA5, 0xA9, 0xAD, 0xB1, 0xB2, 0xB5, 0xB9, 0xBD
	uint8_t val;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_LDA_PTR_ZPG) {
		val = MEMORY_GET_BYTE(cpu->RAM, MEMORY_GET_WORD(cpu->RAM, *((uint8_t*)cpu->arg)));
	} else {
		val = InstructionDecodeAddressingInput(cpu);
	}
	cpu->A = val;
	cpu->F.flags.Z = cpu->A == 0;
	cpu->F.flags.N = (cpu->A & 0x80) == 0x80;
}
void InstructionLDX(CPU* cpu, uint8_t pad) {  // 0xA2, 0xA6, 0xAE, 0xB6, 0xBE
	uint8_t val;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_LDX_IMM)
		val = *((uint8_t*)cpu->arg);
	else
		val = InstructionDecodeAddressingInput(cpu);
	cpu->X = val;
	cpu->F.flags.Z = cpu->X == 0;
	cpu->F.flags.N = (cpu->X & 0x80) == 0x80;
}
void InstructionLDY(CPU* cpu, uint8_t pad) {  // 0xA0, 0xA4, 0xAC, 0xB4, 0xBC
	uint8_t val;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_LDY_IMM)
		val = *((uint8_t*)cpu->arg);
	else
		val = InstructionDecodeAddressingInput(cpu);
	cpu->Y = val;
	cpu->F.flags.Z = cpu->Y == 0;
	cpu->F.flags.N = (cpu->Y & 0x80) == 0x80;
}

// works
void InstructionSTZ(CPU* cpu, uint8_t pad) {  // 0x64, 0x74, 0x9C, 0x9E
	uint16_t dest;
	switch (cpu->currentOpCode) {
		case INSTRUCTION_SPECIAL_STZ_ZPG: dest = *((uint8_t*)cpu->arg); break;
		case INSTRUCTION_SPECIAL_STZ_ZPG_X: dest = (uint8_t)(*((uint8_t*)cpu->arg) + cpu->X); break;
		case INSTRUCTION_SPECIAL_STZ_ABS: dest = *((uint16_t*)cpu->arg); break;
		case INSTRUCTION_SPECIAL_STZ_ABS_X: dest = *((uint16_t*)cpu->arg) + cpu->X; break;
		default: dest = 0;	// should never happen
	}
	MEMORY_SET_BYTE(cpu->RAM, dest, 0);
}
void InstructionSTA(CPU* cpu, uint8_t pad) {  // 0x81, 0x85, 0x8D, 0x91, 0x92, 0x95, 0x9D, 0x99
	uint16_t dest;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_STA_PTR_ZPG)
		dest = MEMORY_GET_WORD(cpu->RAM, *((uint8_t*)cpu->arg));
	else
		dest = InstructionDecodeAddressingDest(cpu);
	MEMORY_SET_BYTE(cpu->RAM, dest, cpu->A);
}
void InstructionSTX(CPU* cpu, uint8_t pad) {  // 0x86, 0x8E, 0x96
	uint16_t dest;
	dest = InstructionDecodeAddressingDest(cpu);
	MEMORY_SET_BYTE(cpu->RAM, dest, cpu->X);
}
void InstructionSTY(CPU* cpu, uint8_t pad) {  // 0x84, 0x8C, 0x94
	uint16_t dest;
	dest = InstructionDecodeAddressingDest(cpu);
	MEMORY_SET_BYTE(cpu->RAM, dest, cpu->Y);
}

void InstructionCMP(CPU* cpu, uint8_t pad) {  // 0xC1, 0xC5, 0xC9, 0xCD, 0xD1, 0xD2, 0xD5, 0xD9, 0xDD
	uint8_t val;
	uint16_t tmp;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_CMP_PTR_ZPG_A)
		val = MEMORY_GET_BYTE(cpu->RAM, *((uint8_t*)cpu->arg));
	else
		val = InstructionDecodeAddressingInput(cpu);
	tmp = (uint16_t)cpu->A - (uint16_t)val;
	cpu->F.flags.C = tmp > 0xFF;
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
}
void InstructionCPX(CPU* cpu, uint8_t pad) {  // 0xE0, 0xE4, 0xEC
	uint8_t val;
	uint16_t tmp;
	val = InstructionDecodeAddressingInput(cpu);
	tmp = (uint16_t)cpu->X - (uint16_t)val;
	cpu->F.flags.C = tmp > 0xFF;
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
}
void InstructionCPY(CPU* cpu, uint8_t pad) {  // 0xC0, 0xC4, 0xCC
	uint8_t val;
	uint16_t tmp;
	val = InstructionDecodeAddressingInput(cpu);
	tmp = (uint16_t)cpu->Y - (uint16_t)val;
	cpu->F.flags.C = tmp > 0xFF;
	cpu->F.flags.Z = ((uint8_t)tmp) == 0;
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
}

void InstructionWAI(CPU* cpu, uint8_t pad) {}  // 0xCB
void InstructionSTP(CPU* cpu, uint8_t pad) {}  // 0xDB

void InstructionINC(CPU* cpu, uint8_t pad) {  // 0x1A, 0xE6, 0xEE, 0xF6, 0xFE
	uint16_t dest;
	uint8_t tmp;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_INC_A) {
		cpu->A++;
		tmp = cpu->A;
	} else {
		dest = InstructionDecodeAddressingDest(cpu);
		tmp = MEMORY_GET_BYTE(cpu->RAM, dest) + 1;
		MEMORY_SET_BYTE(cpu->RAM, dest, tmp);
	}
	cpu->F.flags.Z = tmp == 0;
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
}
void InstructionINX(CPU* cpu, uint8_t pad) {  // 0xE8
	cpu->X++;
	cpu->F.flags.Z = cpu->X == 0;
	cpu->F.flags.N = (cpu->X & 0x80) == 0x80;
}
void InstructionINY(CPU* cpu, uint8_t pad) {  // 0xC8
	cpu->Y++;
	cpu->F.flags.Z = cpu->Y == 0;
	cpu->F.flags.N = (cpu->Y & 0x80) == 0x80;
}

void InstructionDEC(CPU* cpu, uint8_t pad) {  // 0x3A, 0xC6, 0xCE, 0xD6, 0xDE
	uint16_t dest;
	uint8_t tmp;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_DEC_A) {
		cpu->A--;
		tmp = cpu->A;
	} else {
		dest = InstructionDecodeAddressingDest(cpu);
		tmp = MEMORY_GET_BYTE(cpu->RAM, dest) - 1;
		MEMORY_SET_BYTE(cpu->RAM, dest, tmp);
	}
	cpu->F.flags.Z = tmp == 0;
	cpu->F.flags.N = (tmp & 0x80) == 0x80;
}
void InstructionDEX(CPU* cpu, uint8_t pad) {  // 0xCA
	cpu->X--;
	cpu->F.flags.Z = cpu->X == 0;
	cpu->F.flags.N = (cpu->X & 0x80) == 0x80;
}
void InstructionDEY(CPU* cpu, uint8_t pad) {  // 0x88
	cpu->Y--;
	cpu->F.flags.Z = cpu->Y == 0;
	cpu->F.flags.N = (cpu->Y & 0x80) == 0x80;
}

void InstructionORA(CPU* cpu, uint8_t pad) {  // 0x01, 0x05, 0x09, 0x0D, 0x10, 0x11, 0x12, 0x15, 0x19, 0x1D
	uint8_t val;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_ORA_PTR_ZPG_A)
		val = MEMORY_GET_BYTE(cpu->RAM, MEMORY_GET_WORD(cpu->RAM, *((uint8_t*)cpu->arg)));
	else
		val = InstructionDecodeAddressingInput(cpu);
	cpu->A = cpu->A | val;
	cpu->F.flags.Z = cpu->A == 0;			   // affects Z flag
	cpu->F.flags.N = (cpu->A & 0x80) == 0x80;  // affects N flag
}
void InstructionAND(CPU* cpu, uint8_t pad) {  // 0x21, 0x25, 0x29, 0x2D, 0x31, 0x32, 0x35, 0x39, 0x3D
	uint8_t val;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_AND_PTR_ZPG_A)
		val = MEMORY_GET_BYTE(cpu->RAM, MEMORY_GET_WORD(cpu->RAM, *((uint8_t*)cpu->arg)));
	else
		val = InstructionDecodeAddressingInput(cpu);
	cpu->A = cpu->A & val;
	cpu->F.flags.Z = cpu->A == 0;			   // affects Z flag
	cpu->F.flags.N = (cpu->A & 0x80) == 0x80;  // affects N flag
}
void InstructionEOR(CPU* cpu, uint8_t pad) {  // 0x41, 0x45, 0x49, 0x4D, 0x51, 0x52, 0x55, 0x59, 0x5D
	uint8_t val;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_EOR_PTR_ZPG_A)
		val = MEMORY_GET_BYTE(cpu->RAM, MEMORY_GET_WORD(cpu->RAM, *((uint8_t*)cpu->arg)));
	else
		val = InstructionDecodeAddressingInput(cpu);
	cpu->A = cpu->A ^ val;
	cpu->F.flags.Z = cpu->A == 0;			   // affects Z flag
	cpu->F.flags.N = (cpu->A & 0x80) == 0x80;  // affects N flag
}

void InstructionJMP(CPU* cpu, uint8_t pad) {  // 0x4C, 0x6C, 0x7C
	uint16_t val;
	if (cpu->currentOpCode == INSTRUCTION_SPECIAL_JMP_ABS)
		val = *((uint16_t*)cpu->arg);
	else if (cpu->currentOpCode == INSTRUCTION_SPECIAL_JMP_IND)
		val = MEMORY_GET_BYTE(cpu->RAM, MEMORY_GET_WORD(cpu->RAM, *((uint8_t*)cpu->arg)));
	else if (cpu->currentOpCode == INSTRUCTION_SPECIAL_JMP_IND_ABS_X)
		val = MEMORY_GET_WORD(cpu->RAM, MEMORY_GET_WORD(cpu->RAM, (uint8_t)(*((uint8_t*)cpu->arg)) + cpu->X));
	else
		val = 0;
	cpu->PC = val;
	cpu->hasJumped = true;
}
#include <stdio.h>
void InstructionBRA(CPU* cpu, uint8_t pad) {  // 0x80
	int8_t offset = *((int8_t*)cpu->arg);
	cpu->PC += 2;
	cpu->PC += offset;
	cpu->hasJumped = true;
}

void InstructionBMI(CPU* cpu, uint8_t pad) {  // 0x30
	if (cpu->F.flags.N) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBPL(CPU* cpu, uint8_t pad) {  // 0x10
	if (!cpu->F.flags.N) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBVS(CPU* cpu, uint8_t pad) {  // 0x70
	if (cpu->F.flags.V) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBVC(CPU* cpu, uint8_t pad) {  // 0x50
	if (!cpu->F.flags.V) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBCS(CPU* cpu, uint8_t pad) {  // 0xB0
	if (cpu->F.flags.C) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBCC(CPU* cpu, uint8_t pad) {  // 0x90
	if (!cpu->F.flags.C) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBEQ(CPU* cpu, uint8_t pad) {  // 0xF0
	if (cpu->F.flags.Z) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBNE(CPU* cpu, uint8_t pad) {  // 0xD0
	if (!cpu->F.flags.Z) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBBS(CPU* cpu, uint8_t bitPos) {	 // 0x8F, 0x9F, 0xAF, 0xBF, 0xCF, 0xDF, 0xEF, 0xFF
	if (cpu->A & (1 << bitPos)) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}
void InstructionBBR(CPU* cpu, uint8_t bitPos) {	 // 0x0F, 0x1F, 0x2F, 0x3F, 0x4F, 0x5F, 0x6F, 0x7F
	if (!(cpu->A & (1 << bitPos))) {
		int8_t offset = *((int8_t*)cpu->arg);
		cpu->PC += 2;
		cpu->PC += offset;
		cpu->hasJumped = true;
	}
}

void InstructionJSR(CPU* cpu, uint8_t pad) {  // 0x20
	PushWord(cpu, (cpu->PC + 2));
	cpu->PC = *((uint16_t*)cpu->arg);
	cpu->hasJumped = true;
}
void InstructionRTS(CPU* cpu, uint8_t pad) {  // 0x60
	cpu->PC = PopWord(cpu) + 1;
	cpu->hasJumped = true;
}
void InstructionRTI(CPU* cpu, uint8_t pad) {  // 0x40
	cpu->F.reg = PopByte(cpu);
	cpu->F.flags.B = 0;	   // ignore B
	cpu->F.flags.pad = 1;  // ignore bit 5
	cpu->PC = PopWord(cpu);
	cpu->hasJumped = true;
}

void InstructionCLV(CPU* cpu, uint8_t pad) {  // 0xB8
	cpu->F.flags.V = false;
}
void InstructionSEI(CPU* cpu, uint8_t pad) {  // 0x78
	cpu->F.flags.I = true;
}
void InstructionCLI(CPU* cpu, uint8_t pad) {  // 0x58
	cpu->F.flags.I = false;
}
void InstructionSEC(CPU* cpu, uint8_t pad) {  // 0x38
	cpu->F.flags.C = true;
}
void InstructionCLC(CPU* cpu, uint8_t pad) {  // 0x18
	cpu->F.flags.C = false;
}
void InstructionSED(CPU* cpu, uint8_t pad) {  // 0xF8
	cpu->F.flags.D = true;
}
void InstructionCLD(CPU* cpu, uint8_t pad) {  // 0xD8
	cpu->F.flags.D = false;
}

void InstructionPHA(CPU* cpu, uint8_t pad) {  // 0x48
	PushByte(cpu, cpu->A);
}
void InstructionPHX(CPU* cpu, uint8_t pad) {  // 0xDA
	PushByte(cpu, cpu->X);
}
void InstructionPHY(CPU* cpu, uint8_t pad) {  // 0x5A
	PushByte(cpu, cpu->Y);
}
void InstructionPHP(CPU* cpu, uint8_t pad) {  // 0x08
	PushByte(cpu, cpu->F.reg | 0x10 | 0x20);  // B and bit 5 set to 1
}

void InstructionPLA(CPU* cpu, uint8_t pad) {  // 0x68
	cpu->A = PopByte(cpu);
	cpu->F.flags.Z = cpu->A == 0;
	cpu->F.flags.N = (cpu->A & 0x80) == 0x80;
}
void InstructionPLX(CPU* cpu, uint8_t pad) {  // 0xFA
	cpu->X = PopByte(cpu);
	cpu->F.flags.Z = cpu->X == 0;
	cpu->F.flags.N = (cpu->X & 0x80) == 0x80;
}
void InstructionPLY(CPU* cpu, uint8_t pad) {  // 0x7A
	cpu->Y = PopByte(cpu);
	cpu->F.flags.Z = cpu->Y == 0;
	cpu->F.flags.N = (cpu->Y & 0x80) == 0x80;
}
void InstructionPLP(CPU* cpu, uint8_t pad) {  // 0x28
	cpu->F.reg = PopByte(cpu);
	cpu->F.flags.B = 0;	   // ignore B
	cpu->F.flags.pad = 1;  // ignore bit 5
}

const InstructionFunc instructionTable[256] = {
	// 0x0X
	InstructionBRK, InstructionORA, InstructionNOP, InstructionNOP, InstructionTSB, InstructionORA, InstructionASL, InstructionRMB,
	InstructionPHP, InstructionORA, InstructionASL, InstructionNOP, InstructionTSB, InstructionORA, InstructionASL, InstructionBBR,

	// 0x1X
	InstructionBPL, InstructionORA, InstructionORA, InstructionNOP, InstructionTRB, InstructionORA, InstructionASL, InstructionRMB,
	InstructionCLC, InstructionORA, InstructionINC, InstructionNOP, InstructionTRB, InstructionORA, InstructionASL, InstructionBBR,

	// 0x2X
	InstructionJSR, InstructionAND, InstructionNOP, InstructionNOP, InstructionBIT, InstructionAND, InstructionROL, InstructionRMB,
	InstructionPLP, InstructionAND, InstructionROL, InstructionNOP, InstructionBIT, InstructionAND, InstructionROL, InstructionBBR,

	// 0x3X
	InstructionBMI, InstructionAND, InstructionAND, InstructionNOP, InstructionBIT, InstructionAND, InstructionROL, InstructionRMB,
	InstructionSEC, InstructionAND, InstructionDEC, InstructionNOP, InstructionBIT, InstructionAND, InstructionROL, InstructionBBR,

	// 0x4X
	InstructionRTI, InstructionEOR, InstructionNOP, InstructionNOP, InstructionNOP, InstructionEOR, InstructionLSR, InstructionRMB,
	InstructionPHA, InstructionEOR, InstructionLSR, InstructionNOP, InstructionJMP, InstructionEOR, InstructionLSR, InstructionBBR,

	// 0x5X
	InstructionBVC, InstructionEOR, InstructionEOR, InstructionNOP, InstructionNOP, InstructionEOR, InstructionLSR, InstructionRMB,
	InstructionCLI, InstructionEOR, InstructionPHY, InstructionNOP, InstructionNOP, InstructionEOR, InstructionLSR, InstructionBBR,

	// 0x6X
	InstructionRTS, InstructionADC, InstructionNOP, InstructionNOP, InstructionSTZ, InstructionADC, InstructionROR, InstructionRMB,
	InstructionPLA, InstructionADC, InstructionROR, InstructionNOP, InstructionJMP, InstructionADC, InstructionROR, InstructionBBR,

	// 0x7X
	InstructionBVS, InstructionADC, InstructionADC, InstructionNOP, InstructionSTZ, InstructionADC, InstructionROR, InstructionRMB,
	InstructionSEI, InstructionADC, InstructionPLY, InstructionNOP, InstructionJMP, InstructionADC, InstructionROR, InstructionBBR,

	// 0x8X
	InstructionBRA, InstructionSTA, InstructionNOP, InstructionNOP, InstructionSTY, InstructionSTA, InstructionSTX, InstructionSMB,
	InstructionDEY, InstructionBIT, InstructionTXA, InstructionNOP, InstructionSTY, InstructionSTA, InstructionSTX, InstructionBBS,

	// 0x9X
	InstructionBCC, InstructionSTA, InstructionNOP, InstructionNOP, InstructionSTY, InstructionSTA, InstructionSTX, InstructionSMB,
	InstructionTYA, InstructionSTA, InstructionTXS, InstructionNOP, InstructionSTZ, InstructionSTA, InstructionSTZ, InstructionBBS,

	// 0xAX
	InstructionLDY, InstructionLDA, InstructionLDX, InstructionNOP, InstructionLDY, InstructionLDA, InstructionLDX, InstructionSMB,
	InstructionTAY, InstructionLDA, InstructionTAX, InstructionNOP, InstructionLDY, InstructionLDA, InstructionLDX, InstructionBBS,

	// 0xBX
	InstructionBCS, InstructionLDA, InstructionLDA, InstructionNOP, InstructionLDY, InstructionLDA, InstructionLDX, InstructionSMB,
	InstructionCLV, InstructionLDA, InstructionTSX, InstructionNOP, InstructionLDY, InstructionLDA, InstructionLDX, InstructionBBS,

	// 0xCX
	InstructionCPY, InstructionCMP, InstructionNOP, InstructionNOP, InstructionCPY, InstructionCMP, InstructionDEC, InstructionSMB,
	InstructionINY, InstructionCMP, InstructionDEX, InstructionWAI, InstructionCPY, InstructionCMP, InstructionDEC, InstructionBBS,

	// 0xDX
	InstructionBNE, InstructionCMP, InstructionCMP, InstructionNOP, InstructionNOP, InstructionCMP, InstructionDEC, InstructionSMB,
	InstructionCLD, InstructionCMP, InstructionPHX, InstructionSTP, InstructionNOP, InstructionCMP, InstructionDEC, InstructionBBS,

	// 0xEX
	InstructionCPX, InstructionSBC, InstructionNOP, InstructionNOP, InstructionCPX, InstructionSBC, InstructionINC, InstructionSMB,
	InstructionINX, InstructionSBC, InstructionNOP, InstructionNOP, InstructionCPX, InstructionSBC, InstructionINC, InstructionBBS,

	// 0xFX
	InstructionBEQ, InstructionSBC, InstructionSBC, InstructionNOP, InstructionNOP, InstructionSBC, InstructionINC, InstructionSMB,
	InstructionSED, InstructionSBC, InstructionPLX, InstructionNOP, InstructionNOP, InstructionSBC, InstructionINC, InstructionBBS};
