#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "msi_bus.h"
#include "util.h"
#include "IO.h"

#define DRAM_LATENCY 64 
#define DRAM_SIZE 1048576 // 2^20 
static int staticMem[DRAM_SIZE];

typedef struct DRAM{
	int *mem; 
	MSI_BUS *bus;
	char *memoutFilepath;
	int lastAddr;		
	int numOfWaitCycles;
	Clock *clock;
} DRAM;

/*
	just a simple array for dram. Of course it should have init and print log functions.
	need to be able to handle requests form bus
*/

void dram__init(DRAM *DRAM, MSI_BUS *bus, char *meminFilepath, char *memoutFilepath, Clock *clock);
void dram__update(DRAM *DRAM);
void dram__terminate(DRAM *DRAM);