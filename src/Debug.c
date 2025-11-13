#include "Debug.h"

#include <stdio.h>

#include "Instruction.h"
#include "Memory.h"

static uint8_t addressingModeSize[] = {
	1,	// ADDRESSING_MODE_INDEXED_X_INDIRECT = 0,
	1,	// ADDRESSING_MODE_ZEROPAGE = 1,
	1,	// ADDRESSING_MODE_IMMEDIATE = 2,
	2,	// ADDRESSING_MODE_ABSOLUTE = 3,
	1,	// ADDRESSING_MODE_INDIRECT_INDEXED_Y = 4,
	1,	// ADDRESSING_MODE_ZEROPAGE_INDEXED_X = 5,
	2,	// ADDRESSING_MODE_ABSOLUTE_INDEXED_Y = 6,
	2,	// ADDRESSING_MODE_ABSOLUTE_INDEXED_X = 7,
	1,	// ADDRESSING_MODE_ZEROPAGE_INDEXED_Y = 8, 				// used only for ldx and stx
	1,	// ADDRESSING_MODE_RELATIVE = 9,					   // special addressing mode only used by branch instructions
	2,	// ADDRESSING_MODE_INDIRECT = 10,					   // special addressing mode only used by JMP
	0,	// ADDRESSING_MODE_IMPLIED = 11,					   // no addressing at all, used for flag bit instructions and the likes
	1,	// ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE = 12,   // special addressing mode that has no way of being properly decoded
	2,	// ADDRESSING_MODE_X_INDEXED_INDIRECT_ABSOLUTE = 13,  // wtf
};

static void PutInstructionSub(CPU* cpu, const char* instructionName, AddressingMode addressingMode, uint16_t addr) {
	printf("%s ", instructionName);
	switch (addressingMode) {
		case ADDRESSING_MODE_IMMEDIATE: printf("#$%02X", MEMORY_GET_BYTE(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_ABSOLUTE: printf("$%04X\n", MEMORY_GET_WORD(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_ABSOLUTE_INDEXED_X: printf("$%04X,x", MEMORY_GET_WORD(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_ABSOLUTE_INDEXED_Y: printf("$%04X,y", MEMORY_GET_WORD(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_INDIRECT: printf("($%04X)", MEMORY_GET_WORD(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE: printf("($%02X)", MEMORY_GET_BYTE(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_INDEXED_X_INDIRECT: printf("($%02X,x)", MEMORY_GET_BYTE(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_INDIRECT_INDEXED_Y: printf("($%02X),y", MEMORY_GET_BYTE(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_RELATIVE: printf("%d", (int8_t)(MEMORY_GET_BYTE(cpu->RAM, addr + 1) + 2)); break;
		case ADDRESSING_MODE_ZEROPAGE: printf("$%02X", MEMORY_GET_BYTE(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_ZEROPAGE_INDEXED_X: printf("$%02X,x", MEMORY_GET_BYTE(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_ZEROPAGE_INDEXED_Y: printf("$%02X,y", MEMORY_GET_BYTE(cpu->RAM, addr + 1)); break;
		case ADDRESSING_MODE_X_INDEXED_INDIRECT_ABSOLUTE: printf("($%04X,x)", MEMORY_GET_WORD(cpu->RAM, addr + 1)); break;
		default: break;
	}
	printf("\n");
}

uint8_t PutInstruction(CPU* cpu, uint16_t addr) {
	uint8_t opcode = MEMORY_GET_BYTE(cpu->RAM, addr);
	switch (opcode) {
		case INSTRUCTION_SPECIAL_BRK:
			PutInstructionSub(cpu, "BRK", ADDRESSING_MODE_IMMEDIATE, addr);
			return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_PHP:
			PutInstructionSub(cpu, "PHP", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_BPL:
			PutInstructionSub(cpu, "BPL", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_CLC:
			PutInstructionSub(cpu, "CLC", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_JSR:
			PutInstructionSub(cpu, "JSR", ADDRESSING_MODE_ABSOLUTE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE] + 1;
		case INSTRUCTION_SPECIAL_PLP:
			PutInstructionSub(cpu, "PLP", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_BMI:
			PutInstructionSub(cpu, "BMI", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_SEC:
			PutInstructionSub(cpu, "SEC", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_RTI:
			PutInstructionSub(cpu, "RTI", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_PHA:
			PutInstructionSub(cpu, "PHA", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_BVC:
			PutInstructionSub(cpu, "BVC", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_CLI:
			PutInstructionSub(cpu, "CLI", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_RTS:
			PutInstructionSub(cpu, "RTS", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_PLA:
			PutInstructionSub(cpu, "PLA", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_BVS:
			PutInstructionSub(cpu, "BVS", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_SEI:
			PutInstructionSub(cpu, "SEI", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_BRA:
			PutInstructionSub(cpu, "BRA", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_DEY:
			PutInstructionSub(cpu, "DEY", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_BCC:
			PutInstructionSub(cpu, "BCC", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_TYA:
			PutInstructionSub(cpu, "TYA", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_LDY_IMM:
			PutInstructionSub(cpu, "LDY", ADDRESSING_MODE_IMMEDIATE, addr);
			return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_TAY:
			PutInstructionSub(cpu, "TAY", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_BCS:
			PutInstructionSub(cpu, "BCS", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_CLV:
			PutInstructionSub(cpu, "CLV", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_CPY_IMM:
			PutInstructionSub(cpu, "CPY", ADDRESSING_MODE_IMMEDIATE, addr);
			return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_INY:
			PutInstructionSub(cpu, "INY", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_BNE:
			PutInstructionSub(cpu, "BNE", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_CLD:
			PutInstructionSub(cpu, "CLD", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_CPX_IMM:
			PutInstructionSub(cpu, "CPX", ADDRESSING_MODE_IMMEDIATE, addr);
			return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_INX:
			PutInstructionSub(cpu, "INX", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_BEQ:
			PutInstructionSub(cpu, "BEQ", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_SED:
			PutInstructionSub(cpu, "SED", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_JMP_ABS:
			PutInstructionSub(cpu, "JMP", ADDRESSING_MODE_ABSOLUTE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE] + 1;
		case INSTRUCTION_SPECIAL_JMP_IND:
			PutInstructionSub(cpu, "JMP", ADDRESSING_MODE_INDIRECT, addr);
			return addressingModeSize[ADDRESSING_MODE_INDIRECT] + 1;

		case 0x02: PutInstructionSub(cpu, "NOP", ADDRESSING_MODE_IMMEDIATE, addr); return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_ASL_A:
			PutInstructionSub(cpu, "ASL A", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_ORA_PTR_ZPG_A:
			PutInstructionSub(cpu, "ORA", ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_INC_A:
			PutInstructionSub(cpu, "INC A", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case 0x22: PutInstructionSub(cpu, "NOP", ADDRESSING_MODE_IMMEDIATE, addr); return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_ROL_A:
			PutInstructionSub(cpu, "ROL A", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_AND_PTR_ZPG_A:
			PutInstructionSub(cpu, "AND", ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_DEC_A:
			PutInstructionSub(cpu, "DEC A", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case 0x42: PutInstructionSub(cpu, "NOP", ADDRESSING_MODE_IMMEDIATE, addr); return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_LSR_A:
			PutInstructionSub(cpu, "LSR A", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_EOR_PTR_ZPG_A:
			PutInstructionSub(cpu, "EOR", ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_PHY:
			PutInstructionSub(cpu, "PHY", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case 0x62: PutInstructionSub(cpu, "NOP", ADDRESSING_MODE_IMMEDIATE, addr); return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_ROR_A:
			PutInstructionSub(cpu, "ROR A", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_ADC_PTR_ZPG_A:
			PutInstructionSub(cpu, "ADC", ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_PLY:
			PutInstructionSub(cpu, "PLY", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case 0x82: PutInstructionSub(cpu, "NOP", ADDRESSING_MODE_IMMEDIATE, addr); return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_TXA:
			PutInstructionSub(cpu, "TXA", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_STA_PTR_ZPG:
			PutInstructionSub(cpu, "STA", ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_TXS:
			PutInstructionSub(cpu, "TXS", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_LDX_IMM:
			PutInstructionSub(cpu, "LDX", ADDRESSING_MODE_IMMEDIATE, addr);
			return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_TAX:
			PutInstructionSub(cpu, "TAX", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_LDA_PTR_ZPG:
			PutInstructionSub(cpu, "LDA", ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_TSX:
			PutInstructionSub(cpu, "TSX", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case 0xC2: PutInstructionSub(cpu, "NOP", ADDRESSING_MODE_IMMEDIATE, addr); return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_DEX:
			PutInstructionSub(cpu, "DEX", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_CMP_PTR_ZPG_A:
			PutInstructionSub(cpu, "CMP", ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_PHX:
			PutInstructionSub(cpu, "PHX", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case 0xE2: PutInstructionSub(cpu, "NOP", ADDRESSING_MODE_IMMEDIATE, addr); return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_NOP:
			PutInstructionSub(cpu, "NOP", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;
		case INSTRUCTION_SPECIAL_SBC_PTR_ZPG_A:
			PutInstructionSub(cpu, "SBC", ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDIRECT_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_PLX:
			PutInstructionSub(cpu, "PLX", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

			// other weird cases for WDC exclusive instructions:

		case INSTRUCTION_SPECIAL_BIT_IMM_A:
			PutInstructionSub(cpu, "BIT", ADDRESSING_MODE_IMMEDIATE, addr);
			return addressingModeSize[ADDRESSING_MODE_IMMEDIATE] + 1;
		case INSTRUCTION_SPECIAL_BIT_ABS_X_A:
			PutInstructionSub(cpu, "BIT", ADDRESSING_MODE_ABSOLUTE_INDEXED_X, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDEXED_X] + 1;
		case INSTRUCTION_SPECIAL_BIT_ZPG_X_A:
			PutInstructionSub(cpu, "BIT", ADDRESSING_MODE_ZEROPAGE_INDEXED_X, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE_INDEXED_X] + 1;

		case INSTRUCTION_SPECIAL_JMP_IND_ABS_X:
			PutInstructionSub(cpu, "JMP", ADDRESSING_MODE_X_INDEXED_INDIRECT_ABSOLUTE, addr);
			return addressingModeSize[ADDRESSING_MODE_X_INDEXED_INDIRECT_ABSOLUTE] + 1;

		case INSTRUCTION_SPECIAL_BBR0:
			PutInstructionSub(cpu, "BBR0", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBR1:
			PutInstructionSub(cpu, "BBR1", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBR2:
			PutInstructionSub(cpu, "BBR2", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBR3:
			PutInstructionSub(cpu, "BBR3", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBR4:
			PutInstructionSub(cpu, "BBR4", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBR5:
			PutInstructionSub(cpu, "BBR5", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBR6:
			PutInstructionSub(cpu, "BBR6", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBR7:
			PutInstructionSub(cpu, "BBR7", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;

		case INSTRUCTION_SPECIAL_BBS0:
			PutInstructionSub(cpu, "BBS0", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBS1:
			PutInstructionSub(cpu, "BBS1", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBS2:
			PutInstructionSub(cpu, "BBS2", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBS3:
			PutInstructionSub(cpu, "BBS3", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBS4:
			PutInstructionSub(cpu, "BBS4", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBS5:
			PutInstructionSub(cpu, "BBS5", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBS6:
			PutInstructionSub(cpu, "BBS6", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;
		case INSTRUCTION_SPECIAL_BBS7:
			PutInstructionSub(cpu, "BBS7", ADDRESSING_MODE_RELATIVE, addr);
			return addressingModeSize[ADDRESSING_MODE_RELATIVE] + 1;

		case INSTRUCTION_SPECIAL_RMB0:
			PutInstructionSub(cpu, "RMB0", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_RMB1:
			PutInstructionSub(cpu, "RMB1", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_RMB2:
			PutInstructionSub(cpu, "RMB2", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_RMB3:
			PutInstructionSub(cpu, "RMB3", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_RMB4:
			PutInstructionSub(cpu, "RMB4", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_RMB5:
			PutInstructionSub(cpu, "RMB5", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_RMB6:
			PutInstructionSub(cpu, "RMB6", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_RMB7:
			PutInstructionSub(cpu, "RMB7", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;

		case INSTRUCTION_SPECIAL_SMB0:
			PutInstructionSub(cpu, "SMB0", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_SMB1:
			PutInstructionSub(cpu, "SMB1", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_SMB2:
			PutInstructionSub(cpu, "SMB2", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_SMB3:
			PutInstructionSub(cpu, "SMB3", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_SMB4:
			PutInstructionSub(cpu, "SMB4", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_SMB5:
			PutInstructionSub(cpu, "SMB5", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_SMB6:
			PutInstructionSub(cpu, "SMB6", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_SMB7:
			PutInstructionSub(cpu, "SMB7", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;

		case INSTRUCTION_SPECIAL_STP:
			PutInstructionSub(cpu, "STP", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		case INSTRUCTION_SPECIAL_STZ_ZPG:
			PutInstructionSub(cpu, "STZ", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;
		case INSTRUCTION_SPECIAL_STZ_ZPG_X:
			PutInstructionSub(cpu, "STZ", ADDRESSING_MODE_ZEROPAGE_INDEXED_X, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE_INDEXED_X] + 1;
		case INSTRUCTION_SPECIAL_STZ_ABS:
			PutInstructionSub(cpu, "STZ", ADDRESSING_MODE_ABSOLUTE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE] + 1;
		case INSTRUCTION_SPECIAL_STZ_ABS_X:
			PutInstructionSub(cpu, "STZ", ADDRESSING_MODE_ABSOLUTE_INDEXED_X, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE_INDEXED_X] + 1;

		case INSTRUCTION_SPECIAL_TRB_ABS:
			PutInstructionSub(cpu, "TRB", ADDRESSING_MODE_ABSOLUTE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE] + 1;
		case INSTRUCTION_SPECIAL_TRB_ZPG:
			PutInstructionSub(cpu, "TRB", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;

		case INSTRUCTION_SPECIAL_TSB_ABS:
			PutInstructionSub(cpu, "TSB", ADDRESSING_MODE_ABSOLUTE, addr);
			return addressingModeSize[ADDRESSING_MODE_ABSOLUTE] + 1;
		case INSTRUCTION_SPECIAL_TSB_ZPG:
			PutInstructionSub(cpu, "TSB", ADDRESSING_MODE_ZEROPAGE, addr);
			return addressingModeSize[ADDRESSING_MODE_ZEROPAGE] + 1;

		case INSTRUCTION_SPECIAL_WAI:
			PutInstructionSub(cpu, "WAI", ADDRESSING_MODE_IMPLIED, addr);
			return addressingModeSize[ADDRESSING_MODE_IMPLIED] + 1;

		default: break;
	}
	AddressingMode addressingMode = DecodeInstructionB(opcode);
	uint8_t instructionClass = DecodeInstructionA(opcode);
	uint8_t instructionSelector = DecodeInstructionC(opcode);

	switch (instructionClass) {
		case INSTRUCTIONA__ORA_ASL:
			if (instructionSelector == 1)
				PutInstructionSub(cpu, "ORA", addressingMode, addr);
			else if (instructionSelector == 2)
				PutInstructionSub(cpu, "ASL", addressingMode, addr);
			break;
		case INSTRUCTIONA_BIT_AND_ROL:
			if (instructionSelector == 0)
				PutInstructionSub(cpu, "BIT", addressingMode, addr);
			else if (instructionSelector == 1)
				PutInstructionSub(cpu, "AND", addressingMode, addr);
			else if (instructionSelector == 2)
				PutInstructionSub(cpu, "ROL", addressingMode, addr);
			break;
		case INSTRUCTIONA__EOR_LSR:
			if (instructionSelector == 1)
				PutInstructionSub(cpu, "EOR", addressingMode, addr);
			else if (instructionSelector == 2)
				PutInstructionSub(cpu, "LSR", addressingMode, addr);
			break;
		case INSTRUCTIONA__ADC_ROR:
			if (instructionSelector == 1)
				PutInstructionSub(cpu, "ADC", addressingMode, addr);
			else if (instructionSelector == 2)
				PutInstructionSub(cpu, "ROR", addressingMode, addr);
			break;
		case INSTRUCTIONA_STY_STA_STX:
			if (instructionSelector == 0)
				PutInstructionSub(cpu, "STY", addressingMode, addr);
			else if (instructionSelector == 1)
				PutInstructionSub(cpu, "STA", addressingMode, addr);
			else if (instructionSelector == 2) {
				if (addressingMode == ADDRESSING_MODE_ZEROPAGE_INDEXED_X) addressingMode = ADDRESSING_MODE_ZEROPAGE_INDEXED_Y;
				PutInstructionSub(cpu, "STX", addressingMode, addr);
			}
			break;
		case INSTRUCTIONA_LDY_LDA_LDX:
			if (instructionSelector == 0)
				PutInstructionSub(cpu, "LDY", addressingMode, addr);
			else if (instructionSelector == 1)
				PutInstructionSub(cpu, "LDA", addressingMode, addr);
			else if (instructionSelector == 2) {
				if (addressingMode == ADDRESSING_MODE_ZEROPAGE_INDEXED_X) addressingMode = ADDRESSING_MODE_ZEROPAGE_INDEXED_Y;
				PutInstructionSub(cpu, "LDX", addressingMode, addr);
			}
			break;
		case INSTRUCTIONA_CPY_CMP_DEC:
			if (instructionSelector == 0)
				PutInstructionSub(cpu, "CPY", addressingMode, addr);
			else if (instructionSelector == 1)
				PutInstructionSub(cpu, "CMP", addressingMode, addr);
			else if (instructionSelector == 2)
				PutInstructionSub(cpu, "DEC", addressingMode, addr);
			break;
		case INSTRUCTIONA_CPX_SBC_INC:
			if (instructionSelector == 0)
				PutInstructionSub(cpu, "CPX", addressingMode, addr);
			else if (instructionSelector == 1)
				PutInstructionSub(cpu, "SBC", addressingMode, addr);
			else if (instructionSelector == 2)
				PutInstructionSub(cpu, "INC", addressingMode, addr);
			break;
	}
	return addressingModeSize[addressingMode] + 1;
}

void PutCPUState(CPU* cpu) {
	printf("A=$%02X\n", cpu->A);
	printf("X=$%02X\n", cpu->X);
	printf("Y=$%02X\n", cpu->Y);
	printf("S=$%02X\n", cpu->S);
	printf("PC=$%04X\n", cpu->PC);
	printf("(PC)=");
	PutInstruction(cpu, cpu->PC);
	printf("\n");
	printf("P=");

	if (cpu->F.flags.N)
		printf("N");
	else
		printf("-");

	if (cpu->F.flags.V)
		printf("V");
	else
		printf("-");

	printf("-");

	if (cpu->F.flags.B)
		printf("B");
	else
		printf("-");

	if (cpu->F.flags.D)
		printf("D");
	else
		printf("-");

	if (cpu->F.flags.I)
		printf("I");
	else
		printf("-");

	if (cpu->F.flags.Z)
		printf("Z");
	else
		printf("-");

	if (cpu->F.flags.C)
		printf("C");
	else
		printf("-");
	printf("\n");
	printf("I=");
	if (cpu->interruptSig.RESET)
		printf("R");
	else
		printf("-");
	if (cpu->interruptSig.NMI)
		printf("N");
	else
		printf("-");
	if (cpu->interruptSig.IRQ)
		printf("I");
	else
		printf("-");
	printf("\n");

	if (cpu->resetHeld)
		printf("R is held");
	else
		printf("R is not held");
	printf("\n");
}