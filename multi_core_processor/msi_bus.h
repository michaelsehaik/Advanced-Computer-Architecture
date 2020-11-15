#pragma once

#include <stdio.h>

typedef struct MSI_BUS {
	FILE* busTraceFile;
} MSI_BUS;

/*
	struct for bus which is shared by srams and dram (see some fields in pdf).
	has methods for trace, sram/dram send request but they are not always valuated.
	the struct updates after we simulated sram+dram for a clock cycle. 
*/

void bus__update(MSI_BUS *bus);
void bus__init(MSI_BUS *bus, const char *traceFilepath);
void bus__terminate(MSI_BUS *bus);