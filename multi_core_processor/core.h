#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "sram.h"
#include "pipeline.h"

#define REG_FILE_SIZE 16
#define IMEM_SIZE 1024

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

void core__init(Core *core, MSI_BUS* bus, char *imemFilepath, char *traceFilepath, char *dsramFilepath, char *tsramFilepath, char *statsFilepath);
bool core__update(Core *core);
void core__terminate(Core *core);