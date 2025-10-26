#include "Interrupt.h"

#include "Memory.h"
#include "Stack.h"

void HoldReset(CPU* cpu) {
	cpu->resetHeld = true;
	cpu->interruptSig.RESET = true;
}

void ReleaseReset(CPU* cpu) { cpu->resetHeld = false; }

void TriggerInterrupt(CPU* cpu, InterruptType interrupt) {
	switch (interrupt) {
		case INTERRUPT_RESET: cpu->interruptSig.RESET = true; break;
		case INTERRUPT_NMI: cpu->interruptSig.NMI = true; break;
		case INTERRUPT_IRQ: cpu->interruptSig.IRQ = true; break;
	}
}

void ExecuteInterrupt(CPU* cpu) {
	if (cpu->resetHeld) return;
	PushWord(cpu, cpu->PC);
	PushByte(cpu, cpu->F.reg);
	cpu->waitingForInterrupt = false;
	if (cpu->interruptSig.RESET) {
		cpu->PC = MEMORY_GET_WORD(cpu->RAM, VECTOR_RESET);
		cpu->waitingForReset = false;
		cpu->interruptSig = (InterruptSig){0};
	} else if (cpu->interruptSig.NMI) {
		cpu->PC = MEMORY_GET_WORD(cpu->RAM, VECTOR_NMI);
		cpu->isInInterrupt = true;
		cpu->interruptSig.NMI = false;
	} else {
		cpu->PC = MEMORY_GET_WORD(cpu->RAM, VECTOR_IRQ);
		cpu->interruptSig.IRQ = false;
	}
}