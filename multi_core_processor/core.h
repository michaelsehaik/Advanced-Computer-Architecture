#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "sram.h"
#include "pipeline.h"

#define REG_FILE_SIZE 16
#define IMEM_SIZE 1024

#define IMM_REG 1

typedef struct Core {
	SRAM SRAM;
	Pipeline pipeline;
	int registers[REG_FILE_SIZE];
	int Imem[IMEM_SIZE];
	int PC;
	bool waitForSRAM;
	bool waitForWB;
	FILE *traceFile;
	FILE *statsFile;
} Core;

/*
	has 1 sram.
	has a register file.
	has 1 pipeline.
	has PC.
	has Imem.
	main logic function:
		1. checks (somehow) if still in stall and skips if true. (sram should have a funtion we can question if we are waiting for it)
		2. calls update of each stage (input pipline.Q, output pipeline.D so no dependency on other stages)
		3. update trace

	has status struct to keep track over:
		1. number of clock cycles the core was running till halt
		2. number of instructions executed
*/

typedef enum{ // indexes to buffer array
	ADD,
	SUB,
	AND,
	OR,
	XOR,
	MUL,
	SLL,
	SRA,
	SRL,
	BEQ,
	BNE,
	BLT,
	BGT,
	BLE,
	BGE,
	JAL,
	LW,
	SW,
	LL,
	SC,
	HALT,
}OpCodes;

void add(Core *core);
void sub(Core *core);
void and(Core *core);
void or (Core *core);
void xor(Core *core);
void mul(Core *core);
void sll(Core *core);
void sra(Core *core);
void srl(Core *core);
void beq(Core *core);
void bne(Core *core);
void blt(Core *core);
void bgt(Core *core);
void ble(Core *core);
void bge(Core *core);
void jal(Core *core);
void lw(Core *core);
void sw(Core *core);
void ll(Core *core);
void sc(Core *core);
void halt(Core *core);


void core__init(Core *core, MSI_BUS* bus, char *imemFilepath, char *traceFilepath, char *dsramFilepath, char *tsramFilepath, char *statsFilepath);
bool core__update(Core *core);
void core__terminate(Core *core);