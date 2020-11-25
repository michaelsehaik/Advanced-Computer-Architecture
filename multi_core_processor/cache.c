#include <stdio.h>

#include "cache.h"

void cache__update(Cache *cache) {
	// handle bus and update core if waiting
}

void cache__init(Cache *cache, MSI_BUS* bus, char *dsramFilepath, char *tsramFilepath) {
	memset(cache->DSRAM, 0, DSRAM_SIZE * sizeof(int));
	memset(cache->TSRAM, 0, TSRAM_SIZE * sizeof(TSRAM_CELL));
	cache->dsramFilepath = dsramFilepath;
	cache->tsramFilepath = tsramFilepath;
	cache->bus = bus;
	cache->readHitCount = 0;
	cache->writeHitCount = 0;
	cache->readMissCount = 0;
	cache->writeMissCount = 0;
}

void printTSRAM(Cache *cache) {
	FILE *outputFile = NULL;
	fopen_s(&outputFile, cache->tsramFilepath, "w");
	for (int i = 0; i < TSRAM_SIZE; i++) {
		fprintf(outputFile, "%05X%03X\n", cache->TSRAM->msi, cache->TSRAM->tag);
	}
	fclose(outputFile);
}

void cache__terminate(Cache *cache) {
	createFileFromArray(cache->dsramFilepath, cache->DSRAM, DSRAM_SIZE, true);
	printTSRAM(cache);
}