#include <stdbool.h>

#include "util.h"
#include "msi_bus.h"
#include "pipeline.h"
#include "cache.h"
#include "dram.h"
#include "core.h"
#include "IO.h"

/**
* load input files and init structs
*/
void init(char **filepaths, Core cores[], MSI_BUS *bus, DRAM *DRAM, char *memout_file, char *regout_file, Clock *clock) {
	checkFiles(filepaths);
	bus__init(bus, filepaths[BUSTRACE_FILE], clock);
	dram__init(DRAM, bus, filepaths[MEMIN_FILE], filepaths[MEMOUT_FILE], clock);
	for (int i = 0; i < 4; i++) {
		core__init(&cores[i], bus, i,
								filepaths[IMEM_FILE_BASE + i],
								filepaths[CORE_TRACE_FILE_BASE + i],
								filepaths[DSRAM_FILE_BASE + i],
								filepaths[TSRAM_FILE_BASE + i],
								filepaths[STATS_FILE_BASE + i],
								filepaths[REGS_FILE_BASE + i],
								clock);
	}
}

/**
* call bus update methods according to correct order
*/
void busUpdate(MSI_BUS *bus, Core cores[], DRAM *DRAM) {
	bus__update(bus);
	for (int i = 0; i < 4; i++) {
		cache__snoop(&cores[i].cache);
	}
	dram__update(DRAM);
	for (int i = 0; i < 4; i++) {
		cache__update(&cores[i].cache);
	}
	bus__updateTransaction(bus);
}

/**
* main loop
*/
int main(int argc, char **argv) {
	Core cores[4];
	MSI_BUS bus;
	DRAM DRAM;
	char *memout_file = NULL, *regout_file = NULL;
	Clock clock = (Clock){.cycle = 0};
	bool done = false;

	if (argc == 1) init(default_args, cores, &bus, &DRAM, memout_file, regout_file, &clock);
	else init(argv, cores, &bus, &DRAM, memout_file, regout_file, &clock);

	while (!done) {
		done = true;
		busUpdate(&bus, cores, &DRAM);
		for (int i = 0; i < 4; i++) {
			core__update(&cores[i]);
			done &= (cores[i].pipelineIsEmpty); // false if at least one core hasn't finished yet
		}
		clock.cycle++;
	}

	bus__terminate(&bus);
	dram__terminate(&DRAM);
	for (int i = 0; i < 4; i++) {
		core__terminate(&cores[i]);
	}

	return 0;
}