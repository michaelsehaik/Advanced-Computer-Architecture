#pragma once

#include "msi_bus.h"
#include "util.h"

// TODO is this too large for c array?
#define DRAM_SIZE 1048576 // 2^20 

typedef struct DRAM{
	int mem[1]; // TODO: set this with malloc (DRAM_SIZE bytes)
	MSI_BUS *bus;
	char *memoutFilepath;
	int lastAddr;				// TODO needs to hold the last non-zero address
	Clock *clock;
} DRAM;

/*
	just a simple array for dram. Of course it should have init and print log functions.
	need to be able to handle requests form bus
*/

void dram__init(DRAM *DRAM, MSI_BUS *bus, char *meminFilepath, char *memoutFilepath, Clock *clock);
void dram__update(DRAM *DRAM);
void dram__terminate(DRAM *DRAM);