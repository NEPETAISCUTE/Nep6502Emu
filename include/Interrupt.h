#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "CPU.h"

#define VECTOR_NMI 0xFFFA
#define VECTOR_RESET 0xFFFC
#define VECTOR_IRQ 0xFFFE

typedef enum InterruptType {
	INTERRUPT_IRQ = 0,
	INTERRUPT_NMI = 1,
	INTERRUPT_RESET = 2,
} InterruptType;

void TriggerInterrupt(CPU* cpu, InterruptType interrupt);
void ExecuteInterrupt(CPU* cpu);

#endif