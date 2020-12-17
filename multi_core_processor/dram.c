#include "dram.h"

void dram__update(DRAM *DRAM) {
	if (DRAM->bus->txn.command.Q == FLUSH && DRAM->bus->txn.origID.Q != MAIN_MEMORY) {
		DRAM->mem[DRAM->bus->txn.address.Q] = DRAM->bus->txn.data.Q;
	}
	else if (DRAM->bus->txn.command.Q == BUS_RD || DRAM->bus->txn.command.Q == BUS_RDX) {
		if (DRAM->numOfWaitCycles < 0) {
			printf("DRAM: start countdown\n");
			DRAM->numOfWaitCycles = DRAM_LATENCY - 1;
		}
		else if (DRAM->numOfWaitCycles == 0) {
			int data = DRAM->mem[DRAM->bus->txn.address.Q];
			printf("DRAM: sending request\n");
			bus__requestTXN(DRAM->bus, MAIN_MEMORY, FLUSH, DRAM->bus->txn.address.Q, data); //always granted
		}
	}
	DRAM->numOfWaitCycles--;
}


void dram__init(DRAM *DRAM, MSI_BUS *bus, char *meminFilepath, char *memoutFilepath, Clock *clock) {
	DRAM->bus = bus;
	DRAM->clock = clock;
	DRAM->memoutFilepath = memoutFilepath;
	DRAM->mem = staticMem;
	DRAM->numOfWaitCycles = -1;

	FILE *meminFile = NULL;
	fopen_s(&meminFile, meminFilepath, "r");
	DRAM->lastAddr = loadArrayFromFile(meminFile, DRAM->mem, DRAM_SIZE); 
	fclose(meminFile);
}

void dram__terminate(DRAM *DRAM) {
	createFileFromArray(DRAM->memoutFilepath, DRAM->mem, DRAM->lastAddr, true);
}