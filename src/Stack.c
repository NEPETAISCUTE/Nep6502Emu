#include "Stack.h"

#include "Memory.h"

void PushByte(CPU* cpu, uint8_t data) {
	MEMORY_SET_BYTE(cpu->RAM, cpu->S + 0x100, data);
	cpu->S--;
}

void PushWord(CPU* cpu, uint16_t data) {
	MEMORY_SET_BYTE(cpu->RAM, cpu->S + 0x100, HIGHBYTE(data));
	cpu->S--;
	MEMORY_SET_BYTE(cpu->RAM, cpu->S + 0x100, LOWBYTE(data));
	cpu->S--;
}

uint8_t PopByte(CPU* cpu) {
	cpu->S++;
	return MEMORY_GET_BYTE(cpu->RAM, cpu->S + 0x100);
}

uint16_t PopWord(CPU* cpu) {
	cpu->S++;
	uint8_t low = MEMORY_GET_BYTE(cpu->RAM, cpu->S + 0x100);
	cpu->S++;
	uint8_t high = MEMORY_GET_BYTE(cpu->RAM, cpu->S + 0x100);
	return WORDCAT(high, low);
}