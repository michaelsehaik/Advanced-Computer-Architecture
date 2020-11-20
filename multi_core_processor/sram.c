#include <stdio.h>

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
	SRAM->readHitCount = 0;
	SRAM->writeHitCount = 0;
	SRAM->readMissCount = 0;
	SRAM->writeMissCount = 0;
}

void printTSRAM(SRAM *SRAM) {
	FILE *outputFile = NULL;
	fopen_s(&outputFile, SRAM->tsramFilepath, "w");
	for (int i = 0; i < TSRAM_SIZE; i++) {
		fprintf(outputFile, "%05X%03X\n", SRAM->TSRAM->msi, SRAM->TSRAM->tag);
	}
	fclose(outputFile);
}

void sram__terminate(SRAM *SRAM) {
	arrayToFile(SRAM->dsramFilepath, SRAM->DSRAM, DSRAM_SIZE, true);
	printTSRAM(SRAM);
}