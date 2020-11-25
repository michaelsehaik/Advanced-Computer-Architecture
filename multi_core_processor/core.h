#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "cache.h"
#include "pipeline.h"
#include "clock.h"
#include "IO.h"

#define REG_FILE_SIZE 16
#define IMEM_SIZE 1024

#define IMM_REG 1

//TODO copy 2 enums from assembler.c or make new..

typedef struct Core {
	Cache cache;
	Pipeline pipeline;
	int registers[REG_FILE_SIZE];
	int Imem[IMEM_SIZE];
	DQ_FF PC;
	bool waitForCache;
	bool waitForWB;
	FILE *traceFile;
	char *statsFilepath;
	char* regoutFilepath;
	int instructionCount;
	int decodeStallCount;
	int memStallCount;
	Clock *clock;
	bool halt;
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

void core__init(Core *core,
				MSI_BUS* bus,
				char *ImemFilepath,
				char *traceFilepath,
				char *dsramFilepath,
				char *tsramFilepath,
				char *statsFilepath,
				char *regoutFilepath,
				Clock *clock);
bool core__update(Core *core);
void core__terminate(Core *core);

typedef enum { // indexes to buffer array
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
}OpCode;

int calcNeedToJump(Core* core, int R_rs, int R_rt, OpCode opcode);
int calcAluRes(int A, int B, OpCode opcode);
int memoryManage(Core* core);