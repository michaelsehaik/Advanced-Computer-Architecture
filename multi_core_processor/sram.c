#include "sram.h"

void sram__update(SRAM *SRAM) {
	// handle bus and update core if waiting
}

void sram__init(SRAM *SRAM, MSI_BUS* bus, char *dsramFilepath, char *tsramFilepath) {
	memset(SRAM->DSRAM, 0, DSRAM_SIZE * sizeof(int));
	memset(SRAM->TSRAM, 0, TSRAM_SIZE * sizeof(TSRAM_CELL));
	SRAM->dsramFilepath = dsramFilepath;
	SRAM->tsramFilepath = tsramFilepath;
	SRAM->bus = bus;
}

void sram__terminate(SRAM *SRAM) {
	// write dsramFilepath, tsramFilepath files
}