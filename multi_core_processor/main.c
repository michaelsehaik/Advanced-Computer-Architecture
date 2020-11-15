#include <stdbool.h>

#include "util.h"
#include "msi_bus.h"
#include "pipeline.h"
#include "sram.h"
#include "dram.h"
#include "core.h"

/*
	This module creates 4 cores (each creates sram+pipeline), dram, msi_bus.
	It gets input files and calls each module's init methods with correct file names.
	It keeps track over number of cycles simulates.
	It has a while 
*/

void init(char **filepaths, Core cores[], MSI_BUS *bus, DRAM *DRAM, char *memout_file, char *regout_file) {
	// TODO : REGOUT_FILE I don't know if it should be 1 or 4 files.

	bus__init(bus, filepaths[BUSTRACE_FILE]);
	dram__init(DRAM, bus, filepaths[MEMIN_FILE], filepaths[MEMOUT_FILE]);
	for (int i = 0; i < 4; i++) {
		core__init(&cores[i], bus,
								filepaths[IMEM_FILE_BASE + i],
								filepaths[CORE_TRACE_FILE_BASE + i],
								filepaths[DSRAM_FILE_BASE + i],
								filepaths[TSRAM_FILE_BASE + i],
								filepaths[STATS_FILE_BASE + i]);
	}
}

int main(int argc, char **argv) {
	Core cores[4];
	MSI_BUS bus;
	DRAM DRAM;
	char *memout_file = NULL, *regout_file = NULL;
	int numOfCycles = 0;	// this is given as a pointer to all modules
	bool halt = false;

	if (argc == 1) init(default_args, cores, &bus, &DRAM, memout_file, regout_file);
	else init(argv, cores, &bus, &DRAM, memout_file, regout_file);

	while (!halt) {
		halt = true;

		bus__update(&bus);
		dram__update(&DRAM);
		for (int i = 0; i < 4; i++) {
			core__update(&cores[i]);
		}
		// simulate a clock cycle by calling each module's update function (x4 for each active core). notice that order is important.
		// if any of the cores is still running halt = false

		/*
		more specifically:
		handleUpdatesFromBus()	// dram, sram
		performStep()			// for each each core
		updateBus()				// method of bus module- it got requests from cores but need to update according to priority
		writeLog()				// call methods to update trace files
		*/
		numOfCycles++;
	}
	// TODO terminate - free memory and write tracefile
	bus__terminate(&bus);
	dram__terminate(&DRAM);
	for (int i = 0; i < 4; i++)
		core__terminate(&cores[i]);
	//writeOutFiles(); depends on if REGOUT_FILE is 1 or 4 files

	return 0;
}