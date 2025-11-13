#include "CPU.h"

#include <stdio.h>

#include "Instruction.h"
#include "Interrupt.h"
#include "Memory.h"

void CPUInit(CPU* cpu) {
	cpu->fetchCycles = 0;
	cpu->totalFetchCycles = 0;
	cpu->step = STEP_FETCH;
	cpu->isOpcodeFetched = false;
	cpu->waitingForInterrupt = false;
	cpu->waitingForReset = false;
	cpu->resetHeld = false;
	cpu->isInNMI = false;
	cpu->interruptSig.IRQ = false;
	cpu->interruptSig.NMI = false;
	cpu->interruptSig.RESET = false;
}

static void SetFetchCycleCountFromAddressingType(CPU* cpu) {
	AddressingMode addressingMode = DecodeInstructionB(cpu->currentOpCode);
	switch (addressingMode) {
		// 2 fetch cycles
		case ADDRESSING_MODE_IMMEDIATE:
		case ADDRESSING_MODE_ZEROPAGE:
		case ADDRESSING_MODE_ZEROPAGE_INDEXED_X:
		case ADDRESSING_MODE_INDEXED_X_INDIRECT:
		case ADDRESSING_MODE_INDIRECT_INDEXED_Y: cpu->totalFetchCycles = 2; return;

		// 3 fetch cycles
		case ADDRESSING_MODE_ABSOLUTE:
		case ADDRESSING_MODE_ABSOLUTE_INDEXED_X:
		case ADDRESSING_MODE_ABSOLUTE_INDEXED_Y: cpu->totalFetchCycles = 3; return;

		default: cpu->totalFetchCycles = 0;
	}
}

static void SetFetchCycleCount(CPU* cpu) {
	switch (cpu->currentOpCode) {
		// 1 fetch cycle
		// implied
		case INSTRUCTION_SPECIAL_BRK:	 // native
		case INSTRUCTION_SPECIAL_PHP:	 // native
		case INSTRUCTION_SPECIAL_CLC:	 // native
		case INSTRUCTION_SPECIAL_PLP:	 // native
		case INSTRUCTION_SPECIAL_SEC:	 // native
		case INSTRUCTION_SPECIAL_RTI:	 // native
		case INSTRUCTION_SPECIAL_PHA:	 // native
		case INSTRUCTION_SPECIAL_CLI:	 // native
		case INSTRUCTION_SPECIAL_RTS:	 // native
		case INSTRUCTION_SPECIAL_PLA:	 // native
		case INSTRUCTION_SPECIAL_SEI:	 // native
		case INSTRUCTION_SPECIAL_DEY:	 // native
		case INSTRUCTION_SPECIAL_TYA:	 // native
		case INSTRUCTION_SPECIAL_TAY:	 // native
		case INSTRUCTION_SPECIAL_CLV:	 // native
		case INSTRUCTION_SPECIAL_INY:	 // native
		case INSTRUCTION_SPECIAL_CLD:	 // native
		case INSTRUCTION_SPECIAL_INX:	 // native
		case INSTRUCTION_SPECIAL_SED:	 // native
		case INSTRUCTION_SPECIAL_ASL_A:	 // native
		case INSTRUCTION_SPECIAL_ROL_A:	 // native
		case INSTRUCTION_SPECIAL_LSR_A:	 // native
		case INSTRUCTION_SPECIAL_ROR_A:	 // native
		case INSTRUCTION_SPECIAL_TXA:	 // native
		case INSTRUCTION_SPECIAL_TXS:	 // native
		case INSTRUCTION_SPECIAL_TAX:	 // native
		case INSTRUCTION_SPECIAL_TSX:	 // native
		case INSTRUCTION_SPECIAL_DEX:	 // native
		case INSTRUCTION_SPECIAL_NOP:	 // native

		case INSTRUCTION_SPECIAL_INC_A:
		case INSTRUCTION_SPECIAL_DEC_A:
		case INSTRUCTION_SPECIAL_PHX:
		case INSTRUCTION_SPECIAL_PHY:
		case INSTRUCTION_SPECIAL_PLX:
		case INSTRUCTION_SPECIAL_PLY:
		case INSTRUCTION_SPECIAL_STP:
		case INSTRUCTION_SPECIAL_WAI:

		// NOPs 1 byte
		case 0x03:
		case 0x13:
		case 0x23:
		case 0x33:
		case 0x43:
		case 0x53:
		case 0x63:
		case 0x73:
		case 0x83:
		case 0x93:
		case 0xA3:
		case 0xB3:
		case 0xC3:
		case 0xD3:
		case 0xE3:
		case 0xF3:
		case 0x0B:
		case 0x1B:
		case 0x2B:
		case 0x3B:
		case 0x4B:
		case 0x5B:
		case 0x6B:
		case 0x7B:
		case 0x8B:
		case 0x9B:
		case 0xAB:
		case 0xBB:
		case 0xEB:
		case 0xFB: cpu->totalFetchCycles = 1; return;

		// 2 fetch cycles
		// indirect zeropage addressing
		case INSTRUCTION_SPECIAL_ADC_PTR_ZPG_A:
		case INSTRUCTION_SPECIAL_SBC_PTR_ZPG_A:
		case INSTRUCTION_SPECIAL_CMP_PTR_ZPG_A:
		case INSTRUCTION_SPECIAL_AND_PTR_ZPG_A:
		case INSTRUCTION_SPECIAL_EOR_PTR_ZPG_A:
		case INSTRUCTION_SPECIAL_ORA_PTR_ZPG_A:
		case INSTRUCTION_SPECIAL_LDA_PTR_ZPG:
		case INSTRUCTION_SPECIAL_STA_PTR_ZPG:

		// immediate
		case INSTRUCTION_SPECIAL_LDY_IMM:  // native
		case INSTRUCTION_SPECIAL_CPY_IMM:  // native
		case INSTRUCTION_SPECIAL_CPX_IMM:  // native
		case INSTRUCTION_SPECIAL_LDX_IMM:  // native

		case INSTRUCTION_SPECIAL_BIT_IMM_A:

		// zeropage
		case INSTRUCTION_SPECIAL_RMB0:
		case INSTRUCTION_SPECIAL_RMB1:
		case INSTRUCTION_SPECIAL_RMB2:
		case INSTRUCTION_SPECIAL_RMB3:
		case INSTRUCTION_SPECIAL_RMB4:
		case INSTRUCTION_SPECIAL_RMB5:
		case INSTRUCTION_SPECIAL_RMB6:
		case INSTRUCTION_SPECIAL_RMB7:
		case INSTRUCTION_SPECIAL_SMB0:
		case INSTRUCTION_SPECIAL_SMB1:
		case INSTRUCTION_SPECIAL_SMB2:
		case INSTRUCTION_SPECIAL_SMB3:
		case INSTRUCTION_SPECIAL_SMB4:
		case INSTRUCTION_SPECIAL_SMB5:
		case INSTRUCTION_SPECIAL_SMB6:
		case INSTRUCTION_SPECIAL_SMB7:
		case INSTRUCTION_SPECIAL_STZ_ZPG:
		case INSTRUCTION_SPECIAL_TRB_ZPG:
		case INSTRUCTION_SPECIAL_TSB_ZPG:

		// zeropage X indexed
		case INSTRUCTION_SPECIAL_BIT_ZPG_X_A:
		case INSTRUCTION_SPECIAL_STZ_ZPG_X:

		// relative branch
		case INSTRUCTION_SPECIAL_BPL:  // native
		case INSTRUCTION_SPECIAL_BMI:  // native
		case INSTRUCTION_SPECIAL_BVC:  // native
		case INSTRUCTION_SPECIAL_BVS:  // native
		case INSTRUCTION_SPECIAL_BCC:  // native
		case INSTRUCTION_SPECIAL_BCS:  // native
		case INSTRUCTION_SPECIAL_BNE:  // native
		case INSTRUCTION_SPECIAL_BEQ:  // native

		case INSTRUCTION_SPECIAL_BBR0:
		case INSTRUCTION_SPECIAL_BBR1:
		case INSTRUCTION_SPECIAL_BBR2:
		case INSTRUCTION_SPECIAL_BBR3:
		case INSTRUCTION_SPECIAL_BBR4:
		case INSTRUCTION_SPECIAL_BBR5:
		case INSTRUCTION_SPECIAL_BBR6:
		case INSTRUCTION_SPECIAL_BBR7:
		case INSTRUCTION_SPECIAL_BBS0:
		case INSTRUCTION_SPECIAL_BBS1:
		case INSTRUCTION_SPECIAL_BBS2:
		case INSTRUCTION_SPECIAL_BBS3:
		case INSTRUCTION_SPECIAL_BBS4:
		case INSTRUCTION_SPECIAL_BBS5:
		case INSTRUCTION_SPECIAL_BBS6:
		case INSTRUCTION_SPECIAL_BBS7:
		case INSTRUCTION_SPECIAL_BRA:

		// NOPs 2 bytes
		case 0x02:
		case 0x22:
		case 0x42:
		case 0x62:
		case 0x82:
		case 0xC2:
		case 0xE2:

		case 0x44:

		case 0x54:
		case 0xF4: cpu->totalFetchCycles = 2; return;

		// absolute
		case INSTRUCTION_SPECIAL_JSR:	   // native
		case INSTRUCTION_SPECIAL_JMP_ABS:  // native

		case INSTRUCTION_SPECIAL_STZ_ABS:
		case INSTRUCTION_SPECIAL_TRB_ABS:
		case INSTRUCTION_SPECIAL_TSB_ABS:
		// absolute x indexed
		case INSTRUCTION_SPECIAL_STZ_ABS_X:
		case INSTRUCTION_SPECIAL_BIT_ABS_X_A:
		// indirect absolute addressing
		case INSTRUCTION_SPECIAL_JMP_IND:  // native

		case INSTRUCTION_SPECIAL_JMP_IND_ABS_X:

		// 3 fetch cycles
		// NOPs 3 bytes
		case 0xDC:
		case 0xFC:

		case 0x5C: cpu->totalFetchCycles = 3; return;

		default: SetFetchCycleCountFromAddressingType(cpu); return;
	}
}

static void FetchOpcodeArg(CPU* cpu) { cpu->arg[cpu->fetchCycles - 1] = MEMORY_GET_BYTE(cpu->RAM, cpu->PC + cpu->fetchCycles); }

static void ExecuteInstruction(CPU* cpu) {
	uint8_t bitPos = (cpu->currentOpCode >> 4) % 8;
	(instructionTable[cpu->currentOpCode])(cpu, bitPos);
	cpu->isOpcodeFetched = false;
	if (!cpu->hasJumped)
		cpu->PC += cpu->totalFetchCycles;
	else
		cpu->hasJumped = false;
	cpu->fetchCycles = 0;
	cpu->totalFetchCycles = 0;
	cpu->step = STEP_FETCH;
}

void CPURunCycle(CPU* cpu) {
	if (cpu->resetHeld) return;
	switch (cpu->step) {
		case STEP_FETCH:
			if (!cpu->isOpcodeFetched) {
				if ((cpu->interruptSig.RESET || cpu->interruptSig.NMI || (cpu->interruptSig.IRQ && !cpu->F.flags.I))) {
					ExecuteInterrupt(cpu);
					return;
				}
				cpu->currentOpCode = MEMORY_GET_BYTE(cpu->RAM, cpu->PC);
				cpu->isOpcodeFetched = true;
				SetFetchCycleCount(cpu);
			} else {
				FetchOpcodeArg(cpu);
			}
			cpu->fetchCycles++;
			if (cpu->fetchCycles >= cpu->totalFetchCycles) {
				cpu->step++;
				if (false /*cpu->totalFetchCycles == 0*/) {
					ExecuteInstruction(cpu);
				}
			}
			return;
		case STEP_EXECUTE: ExecuteInstruction(cpu); return;
	}
}