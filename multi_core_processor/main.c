#include <stdbool.h>

#include "clock.h"
#include "msi_bus.h"
#include "pipeline.h"
#include "cache.h"
#include "dram.h"
#include "core.h"
#include "IO.h"

void init(char **filepaths, Core cores[], MSI_BUS *bus, DRAM *DRAM, char *memout_file, char *regout_file, Clock *clock) {

	checkFiles(filepaths);
	bus__init(bus, filepaths[BUSTRACE_FILE], clock);
	dram__init(DRAM, bus, filepaths[MEMIN_FILE], filepaths[MEMOUT_FILE], clock);
	for (int i = 0; i < 4; i++) {
		core__init(&cores[i], bus,
								filepaths[IMEM_FILE_BASE + i],
								filepaths[CORE_TRACE_FILE_BASE + i],
								filepaths[DSRAM_FILE_BASE + i],
								filepaths[TSRAM_FILE_BASE + i],
								filepaths[STATS_FILE_BASE + i],
								filepaths[REGS_FILE_BASE + i],
								clock);
	}
}

int main(int argc, char **argv) {
	Core cores[4];
	MSI_BUS bus;
	DRAM DRAM;
	char *memout_file = NULL, *regout_file = NULL;
	Clock clock = (Clock){.cycle = 0};
	bool halt = false;

	if (argc == 1) init(default_args, cores, &bus, &DRAM, memout_file, regout_file, &clock);
	else init(argv, cores, &bus, &DRAM, memout_file, regout_file, &clock);
	
	while (!halt) {
		halt = true; // if any of the cores is still running halt = false

		bus__update(&bus);
		dram__update(&DRAM);
		for (int i = 0; i < 4; i++) {
			core__update(&cores[i]);
		}
		
		clock.cycle++;
	}

	// TODO terminate - free memory, close files and write tracefile
	bus__terminate(&bus);
	dram__terminate(&DRAM);
	for (int i = 0; i < 4; i++)
		core__terminate(&cores[i]);

	return 0;
}