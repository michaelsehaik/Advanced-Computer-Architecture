#pragma once

#include <stdio.h>

#include "clock.h"
#include "IO.h"

#define WORD_ADDRESS_MASK 0xFFFFF // 20 bits

enum OriginatorID {
	CORE0, CORE1, CORE2, CORE3, MAIN_MEMORY
};

enum BusCommand {
	NO_COMMAND, BUS_RD, BUSRDX, FLUSH
};

typedef struct MSI_BUS {
	FILE* traceFile;
	enum OriginatorID origID;
	enum BusCommand command;
	int address;
	int data;
	Clock *clock;
} MSI_BUS;

/*
	struct for bus which is shared by srams and dram (see some fields in pdf).
	has methods for trace, sram/dram send request but they are not always valuated.
	the struct updates after we simulated sram+dram for a clock cycle. 
*/

void bus__update(MSI_BUS *bus);
void bus__init(MSI_BUS *bus, char *traceFilepath, Clock *clock);
void bus__terminate(MSI_BUS *bus);