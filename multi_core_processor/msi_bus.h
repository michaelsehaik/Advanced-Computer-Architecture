#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "util.h"
#include "IO.h"

#define WORD_ADDRESS_MASK 0xFFFFF // 20 bits

typedef enum OriginatorID {
	CORE0, CORE1, CORE2, CORE3, MAIN_MEMORY
} OriginatorID;

typedef enum BusCommand {
	NO_COMMAND, BUS_RD, BUS_RDX, FLUSH
} BusCommand;

typedef struct BusTransaction {
	DQ_FF origID;
	DQ_FF command;
	DQ_FF address;
	DQ_FF data;
} BusTransaction;

typedef struct MSI_BUS {
	FILE* traceFile;
	bool gotNewReq;
	bool isCommandDone;
	BusTransaction txn;
	Clock *clock;
} MSI_BUS;

/*
	struct for bus which is shared by srams and dram (see some fields in pdf).
	has methods for trace, sram/dram send request but they are not always valuated.
	the struct updates after we simulated sram+dram for a clock cycle. 
*/

void bus__updateTransaction(MSI_BUS *bus);
bool bus__requestTXN(MSI_BUS *bus, OriginatorID origID, BusCommand command, int address, int data);
void bus__update(MSI_BUS *bus);
void bus__init(MSI_BUS *bus, char *traceFilepath, Clock *clock);
void bus__terminate(MSI_BUS *bus);