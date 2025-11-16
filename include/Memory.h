#ifndef MEMORY_H
#define MEMORY_H

#include <CPU.h>

#define WORDCAT(low, high) (uint16_t)(((high) << 8) | (low))
#define HIGHBYTE(data) (((data) & 0xFF00) >> 8)
#define LOWBYTE(data) ((data) & 0xFF)

// #define SWAP16(data) ((((data) & 0xFF) << 8) | (((data) & 0xFF00) >> 8))

#define MEMORY_GET_BYTE(cpu, address) ((uint8_t)((cpu)->readMem(address)))
#define MEMORY_GET_WORD(memory, address) WORDCAT((cpu)->readMem((address)), ((cpu)->readMem((address) + 1)))

#define MEMORY_SET_BYTE(memory, address, data) ((cpu)->writeMem((address), (data)))
#define MEMORY_SET_WORD(memory, address, data) ((cpu)->writeMem((address), LOWBYTE(data)); (cpu)->writeMem((address) + 1, HIGHBYTE(data)))

#endif	// MEMORY_H