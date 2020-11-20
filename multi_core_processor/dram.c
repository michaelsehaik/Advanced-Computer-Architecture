#include "dram.h"
#include "util.h"

void dram__update(DRAM *DRAM) {
	// check if bus transaction is read/write to dram
}


void dram__init(DRAM *DRAM, MSI_BUS *bus, char *meminFilepath, char *memoutFilepath, Clock *clock) {
	DRAM->bus = bus;
	DRAM->clock = clock;
	DRAM->memoutFilepath = memoutFilepath;
	memset(DRAM->mem, 0, DRAM_SIZE * sizeof(int));

	FILE *meminFile = NULL;
	fopen_s(&meminFile, meminFilepath, "r");
	DRAM->lastAddr = loadArrayFromFile(meminFile, DRAM->mem, DRAM_SIZE);
	fclose(meminFile);
}

void dram__terminate(DRAM *DRAM) {
	// create trace file for memoutFilepath
	arrayToFile(DRAM->memoutFilepath, DRAM->mem, DRAM->lastAddr, true);
}