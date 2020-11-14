#include <stdbool.h>

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

int main(int argc, char **argv) {
	int numOfCycles = 0;	// this is given as a pointer to all modules
	bool halt = false;

	while (!halt) {
		halt = true;
		// simulate a clock cycle by calling each module's update function (x4 for each active core). notice that order is important.
		// if any of the cores is still running halt = false

		/*
		more specifically:
		handleUpdatesFromBus()	// dram, sram
		performStep()			// for each each core
		updateBus()				// method of bus module- it got requests from cores but need to update according to priority
		writeLog()				// call methods to update trace files
		*/
		 
	}
	// need to create/save/close output files now. call appropiate methods of modules.

	return 0;
}