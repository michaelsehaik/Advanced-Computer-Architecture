#include "dram.h"
#include "util.h"

void dram__update(DRAM *DRAM) {
	// check if bus transaction is read/write to dram
}


void dram__init(DRAM *DRAM, MSI_BUS *bus, char *meminFilepath, char *memoutFilepath) {
	DRAM->bus = bus;
	DRAM->memoutFilepath = memoutFilepath;

	FILE *meminFile = NULL;
	fopen_s(&meminFile, meminFilepath, "r", DRAM_SIZE);
	memset(DRAM->mem, 0, DRAM_SIZE*sizeof(int));
	loadArrayFromFile(meminFile, DRAM->mem, DRAM_SIZE);
	fclose(meminFile);
}

void dram__terminate(DRAM *DRAM) {
	// create trace file for memoutFilepath
}