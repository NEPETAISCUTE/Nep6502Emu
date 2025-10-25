#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdint.h>

typedef enum CycleStep {
	STEP_FETCH = 0,
	STEP_EXECUTE = 1,
} CycleStep;

typedef union Flags {
	struct flags {
		bool C : 1;
		bool Z : 1;
		bool I : 1;
		bool D : 1;
		bool B : 1;
		bool pad : 1;
		bool V : 1;
		bool N : 1;
	} flags;
	uint8_t reg;
} Flags;

typedef struct InterruptSig {
	bool IRQ : 1;
	bool RESET : 1;
	bool NMI : 1;
} InterruptSig;

typedef struct CPU {
	uint8_t A;
	uint8_t X;
	uint8_t Y;
	uint8_t S;
	uint16_t PC;
	Flags F;

	uint8_t* RAM;

	CycleStep step;

	bool isOpcodeFetched;
	uint8_t currentOpCode;
	uint8_t arg[2];

	uint8_t fetchCycles;
	uint8_t totalFetchCycles;
	bool resetHeld;

	bool hasJumped;
	bool waitingForInterrupt;
	bool waitingForReset;

	InterruptSig interruptSig;
} CPU;

void CPURunCycle(CPU* cpu);
void CPUPutState(CPU* cpu);

#endif	// CPU_H