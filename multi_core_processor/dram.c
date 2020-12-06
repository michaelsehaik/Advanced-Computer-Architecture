#include "dram.h"

void dram__update(DRAM *DRAM) {
	if (DRAM->bus->txn.command == FLUSH && DRAM->bus->txn.origID != MAIN_MEMORY) {
		DRAM->mem[DRAM->bus->txn.address] = DRAM->bus->txn.data;
	}
	else if (DRAM->bus->txn.command == BUS_RD || DRAM->bus->txn.command == BUS_RDX) {
		if (DRAM->numOfWaitCycles < 0) {
			DRAM->numOfWaitCycles = DRAM_LATENCY - 1;
		}
		else if (DRAM->numOfWaitCycles == 0) {
			BusTransaction txn;
			int data = DRAM->mem[DRAM->bus->txn.address];
			bus__createTransaction(&txn, MAIN_MEMORY, FLUSH, DRAM->bus->txn.address, data);
			bus__setTransaction(DRAM->bus, txn);
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