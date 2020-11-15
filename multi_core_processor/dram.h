#pragma once

#include "msi_bus.h"

// TODO is this too large for c array?
#define DRAM_SIZE 1048576 // 2^20 

typedef struct DRAM{
	int mem[DRAM_SIZE];
	MSI_BUS *bus;
	char *memoutFilepath;
} DRAM;

/*
	just a simple array for dram. Of course it should have init and print log functions.
	need to be able to handle requests form bus
*/

void dram__init(DRAM *DRAM, MSI_BUS *bus, char *meminFilepath, char *memoutFilepath);
void dram__update(DRAM *DRAM);
void dram__terminate(DRAM *DRAM);