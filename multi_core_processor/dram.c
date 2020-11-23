#include "dram.h"

void dram__update(DRAM *DRAM) {
	// check if bus transaction is read/write to dram
}


void dram__init(DRAM *DRAM, MSI_BUS *bus, char *meminFilepath, char *memoutFilepath, Clock *clock) {
	DRAM->bus = bus;
	DRAM->clock = clock;
	DRAM->memoutFilepath = memoutFilepath;
	//DRAM->mem = (int*)calloc(DRAM_SIZE, sizeof(int));	// FIXME

	FILE *meminFile = NULL;
	fopen_s(&meminFile, meminFilepath, "r");
	//DRAM->lastAddr = loadArrayFromFile(meminFile, DRAM->mem, DRAM_SIZE); // FIXME
	fclose(meminFile);
}

void dram__terminate(DRAM *DRAM) {
	// create trace file for memoutFilepath
	createFileFromArray(DRAM->memoutFilepath, DRAM->mem, DRAM->lastAddr, true);
	//free(DRAM->mem); // FIXME
}