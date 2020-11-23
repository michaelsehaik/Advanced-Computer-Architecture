#pragma once

#include "msi_bus.h"
#include "clock.h"
#include "IO.h"

#define DRAM_SIZE 1048576 // 2^20 

typedef struct DRAM{
	int *mem; 
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