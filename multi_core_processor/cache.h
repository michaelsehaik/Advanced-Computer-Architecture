#pragma once

#include "msi_bus.h"
#include "clock.h"
#include "IO.h"

#define DSRAM_SIZE 256
#define TSRAM_SIZE 256

#define TAG_WIDTH 12
#define MSI_WIDTH 2

enum MSI_STATE {
	INVALID,
	SHARED,
	MODIFIED
};

typedef struct TSRAM_CELL {
	short tag : TAG_WIDTH;
	enum MSI_STATE msi : MSI_WIDTH;
} TSRAM_CELL;

typedef struct Cache {
	MSI_BUS* bus;
	int DSRAM[DSRAM_SIZE];
	TSRAM_CELL TSRAM[TSRAM_SIZE];
	char *dsramFilepath;
	char *tsramFilepath;
	int readHitCount;
	int writeHitCount;
	int readMissCount;
	int writeMissCount;
} Cache;

/*
	structs for DSRAM and TSRAM.
	this module need also a pointer to the bus struct.
	methods for handling requests from cores, and method to handle bus updates (generate a request, or answer one if it has the data)
	these modules will have in the future logic to handle cache misses.
	has status struct to keep track over:
		1. number of cache read hits
		2. number of cache write hits
		3. number of cache read misses
		4. number of cache write misses
*/

void cache__init(Cache *cache, MSI_BUS* bus, char *dsramFilepath, char *tsramFilepath);
void cache__update(Cache *cache);
void cache__terminate(Cache *cache);