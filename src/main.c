#include <stdio.h>

#include "CPU.h"
#include "Debug.h"
#include "Interrupt.h"

uint8_t RAM[0x6000];
uint8_t ROM[0x8000];

uint16_t getHex(uint8_t digitCnt) {
	uint16_t result = 0;
	for (uint8_t i = 0; i < digitCnt; i++) {
		char c = getchar();
		if (c >= '0' && c <= '9')
			result = (result << 4) | (c - '0');
		else if (c >= 'A' && c <= 'F')
			result = (result << 4) | (c - 'A' + 10);
		else {
			fflush(stdin);
			return 0;
		}
	}
	return result;
}

bool dumpFileIntoMem(const char* filename, uint16_t address, uint8_t* RAM) {
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

uint8_t onCPURead(uint16_t address) {
	if (address < 0x6000) {
		return RAM[address];
	} else if (address < 0x8000) {
		return getchar();
	} else {
		return ROM[address - 0x8000];
	}
}

void onCPUWrite(uint16_t address, uint8_t data) {
	if (address < 0x6000) {
		RAM[address] = data;
	} else if (address < 0x8000) {
		putchar(data);
		return;
	} else {
		return;
	}
}

int main(int argc, char** argv) {
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

	if (argc > 1) dumpFileIntoMem(argv[1], 0, ROM);

	CPU cpu;
	CPUInit(&cpu, onCPURead, onCPUWrite);
	TriggerInterrupt(&cpu, INTERRUPT_RESET);
	bool shouldQuit = false;

	while (!shouldQuit) {
		CPURunCycle(&cpu);
	}
	return 0;
} /*
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
			 PutCPUState(&cpu);
		 }
		 if (c == 'z') {
			 for (size_t j = 0; j < 0x10; j++) {
				 for (size_t i = 0; i < 0x10; i++) {
					 printf("%02X ", onCPURead(i + j * 0x10));
				 }
				 printf("\n");
			 }
		 }
		 if (c == 's') {
			 for (size_t j = 0; j < 0x10; j++) {
				 for (size_t i = 0; i < 0x10; i++) {
					 printf("%02X ", onCPURead(0x100 + i + j * 0x10));
				 }
				 printf("\n");
			 }
		 }
		 if (c == 'r') {
			 uint16_t breakAddr;
			 breakAddr = getHex(4);
			 do {
				 CPURunCycle(&cpu);
			 } while (cpu.PC != breakAddr || cpu.isOpcodeFetched);
		 }
		 if (c == '*') {
			 uint16_t readAddr;
			 readAddr = getHex(4);
			 printf("0x%02X\n", onCPURead(readAddr));
		 }
		 if (c == 'R') {
			 TriggerInterrupt(&cpu, INTERRUPT_RESET);
		 }
		 if (c == 'N') {
			 TriggerInterrupt(&cpu, INTERRUPT_NMI);
		 }
		 if (c == 'I') {
			 TriggerInterrupt(&cpu, INTERRUPT_IRQ);
		 }*/