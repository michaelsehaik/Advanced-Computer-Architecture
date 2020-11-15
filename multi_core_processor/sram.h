#pragma once

struct DSRAM {
	int x;
};

struct TSRAM {
	int x;
};

typedef struct SRAM {
	MSI_BUS* bus;
	struct DSRAM DSRAM;
	struct TSRAM TSRAM;
} SRAM;

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