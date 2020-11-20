#include "msi_bus.h"

void updateBusTrace(MSI_BUS *bus) {
	fprintf(bus->traceFile, "%d %X %X %05X %08X", 
			bus->clock->cycle, bus->origID, bus->command, bus->address, bus->data);
}

void bus__update(MSI_BUS *bus) {
	// TODO not sure what should be here besides update to trace. maybe need to handle issued requests.
	updateBusTrace(bus);
}

void bus__init(MSI_BUS *bus, char *traceFilepath, Clock *clock) {
	// TODO
	bus->clock = clock;
	fopen_s(&bus->traceFile, traceFilepath, "w");
}

void bus__terminate(MSI_BUS *bus) {
	fclose(bus->traceFile);
}