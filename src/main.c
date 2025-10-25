#include <stdio.h>

#include "CPU.h"

bool dumpFileIntoRAM(const char* filename, uint16_t address, uint8_t* RAM) {
	FILE* f = fopen(filename, "rb");
	if (f == NULL) return false;

	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (len > 0x10000 - address) {
		fclose(f);
		return false;
	}

	while (len > 0) len -= fread(RAM + address, 1, len, f);

	fclose(f);
	return true;
}

int main(int argc, char** argv) {
	uint8_t RAM[0x10000];

	/*
	RAM[0x8000] = 0xEA;	 // NOP

	RAM[0x8001] = 0xA9;
	RAM[0x8002] = 0xDE;	 // LDA #$DE

	RAM[0x8003] = 0xA2;
	RAM[0x8004] = 0xEF;	 // LDX #$EF

	RAM[0x8005] = 0x9A;	 // TXS

	RAM[0x8006] = 0xA2;
	RAM[0x8007] = 0xAD;	 // LDX #$AD

	RAM[0x8008] = 0xA0;
	RAM[0x8009] = 0xBE;	 // LDY #$BE

	// loop:
	RAM[0x800A] = 0x80;	 // BRA loop
	RAM[0x800B] = -2;
	*/
	if (argc > 1) dumpFileIntoRAM(argv[1], 0x8000, RAM);

	CPU cpu;
	cpu.PC = 0x8000;

	void CPUInit(CPU * cpu);
	cpu.RAM = RAM;
	bool shouldQuit = false;
	while (!shouldQuit) {
		printf("> ");
		char c = '\0';
		do {
			c = getchar();
			if (c == '\n') c = '\0';
		} while (c == '\0');
		if (c == 'q') shouldQuit = true;
		if (c == 's') CPURunCycle(&cpu);
		if (c == 'n') {
			do {
				CPURunCycle(&cpu);
			} while (cpu.isOpcodeFetched);
		}
		if (c == 'd') {
			CPUPutState(&cpu);
		}
		if (c == 'z') {
			for (size_t j = 0; j < 0x10; j++) {
				for (size_t i = 0; i < 0x10; i++) {
					printf("%02X ", RAM[i + j * 0x10]);
				}
				printf("\n");
			}
		}
		if (c == 's') {
			for (size_t j = 0; j < 0x10; j++) {
				for (size_t i = 0; i < 0x10; i++) {
					printf("%02X ", RAM[0x100 + i + j * 0x10]);
				}
				printf("\n");
			}
		}
		if (c == 'r') {
			uint16_t breakAddr;
			scanf("%04X", &breakAddr);
			while (cpu.PC != breakAddr) {
				CPURunCycle(&cpu);
			}
		}
		if (c == '*') {
			uint16_t readAddr;
			scanf("%04X", &readAddr);
			printf("0x%02X\n", RAM[readAddr]);
		}
	}
	return 0;
}