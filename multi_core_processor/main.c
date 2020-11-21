#include <stdbool.h>

#include "util.h"
#include "msi_bus.h"
#include "pipeline.h"
#include "sram.h"
#include "dram.h"
#include "core.h"

#define IMEM_FILE_BASE				1
#define MEMIN_FILE					5
#define MEMOUT_FILE					6
#define REGS_FILE_BASE				7
#define CORE_TRACE_FILE_BASE		11
#define BUSTRACE_FILE				15
#define DSRAM_FILE_BASE				16
#define TSRAM_FILE_BASE				20
#define STATS_FILE_BASE				24

static char* default_args[] = { "sim.exe",
								"imem0.txt", "imem1.txt", "imem2.txt", "imem3.txt",
								"memin.txt", "memout.txt",
								"regout0.txt", "regout1.txt", "regout2.txt", "regout3.txt",
								"core0trace.txt", "core1trace.txt", "core2trace.txt", "core3trace.txt",
								"bustrace.txt",
								"dsram0.txt", "dsram1.txt", "dsram2.txt", "dsram3.txt",
								"tsram0.txt", "tsram1.txt", "tsram2.txt", "tsram3.txt",
								"stats0.txt", "stats1.txt", "stats2.txt", "stats3.txt",
								0 };

void init(char **filepaths, Core cores[], MSI_BUS *bus, DRAM *DRAM, char *memout_file, char *regout_file, Clock *clock) {

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