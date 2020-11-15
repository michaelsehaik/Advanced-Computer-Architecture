#pragma once

#include <stdbool.h>

#include "sram.h"
#include "pipeline.h"

typedef struct Core {
	SRAM SRAM;
	Pipeline pipeline;
	int registers[16];
	int Imem[1024];
	int PC;
	bool waitForSRAM;
	bool waitForWB;
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