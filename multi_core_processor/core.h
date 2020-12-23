#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "cache.h"
#include "pipeline.h"
#include "util.h"
#include "IO.h"

#define REG_FILE_SIZE 16
#define IMEM_SIZE 1024

#define IMM_REG 1

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
} OpCode;

typedef struct Core {
	Cache cache;
	Pipeline pipeline;
	DQ_FF registers[REG_FILE_SIZE];
	int Imem[IMEM_SIZE];
	DQ_FF PC;
	bool waitForCache;
	bool waitForWB;
	FILE *traceFile;
	char *statsFilepath;
	char* regoutFilepath;
	int instructionCount;
	Clock *clock;
	bool halt;
	bool pipelineIsEmpty;
	RegisterDMA linkRegister;
} Core;

void core__init(Core *core,
				MSI_BUS* bus,
				OriginatorID origID,
				char *ImemFilepath,
				char *traceFilepath,
				char *dsramFilepath,
				char *tsramFilepath,
				char *statsFilepath,
				char *regoutFilepath,
				Clock *clock);
void core__update(Core *core);
void core__terminate(Core *core);

