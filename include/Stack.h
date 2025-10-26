#ifndef STACK_H
#define STACK_H

#include <CPU.h>

void PushByte(CPU* cpu, uint8_t data);
void PushWord(CPU* cpu, uint16_t data);

uint8_t PopByte(CPU* cpu);
uint16_t PopWord(CPU* cpu);

#endif