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

/**
* init dram struct
*/
void dram__init(DRAM *DRAM, MSI_BUS *bus, char *meminFilepath, char *memoutFilepath, Clock *clock);
/**
* handle bus transaction. return data on rd, store data on flush
*/
void dram__update(DRAM *DRAM);
/**
* write output files on termination
*/
void dram__terminate(DRAM *DRAM);