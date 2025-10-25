#ifndef MEMORY_H
#define MEMORY_H

#define WORDCAT(high, low) (uint16_t)(((high) << 8) | (low))
#define HIGHBYTE(data) (((data) & 0xFF00) >> 8)
#define LOWBYTE(data) ((data) & 0xFF)

// #define SWAP16(data) ((((data) & 0xFF) << 8) | (((data) & 0xFF00) >> 8))

#define MEMORY_GET_BYTE(memory, address) (*(uint8_t*)(((uint8_t*)memory) + ((uint16_t)address)))
#define MEMORY_GET_WORD(memory, address) (*(uint16_t*)(((uint16_t*)memory) + ((uint16_t)address)))

#define MEMORY_SET_BYTE(memory, address, data) ((*(uint8_t*)(((uint8_t*)memory) + ((uint16_t)address))) = ((uint8_t)data))
#define MEMORY_SET_WORD(memory, address, data) ((*(uint16_t*)(((uint8_t*)memory) + ((uint16_t)address))) = (uint16_t)data)

#endif	// MEMORY_H