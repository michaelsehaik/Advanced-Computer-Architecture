#include "msi_bus.h"

void updateBusTrace(MSI_BUS *bus) {
	//TODO
}

void bus__update(MSI_BUS *bus) {
	// TODO not sure what should be here besides update to trace. maybe need to handle issued requests.
	updateBusTrace(bus);
}

void bus__init(MSI_BUS *bus, const char *traceFilepath) {
	// TODO
	fopen_s(&bus->busTraceFile, traceFilepath, "w");
}

void bus__terminate(MSI_BUS *bus) {
	fclose(bus->busTraceFile);
}