#include "msi_bus.h"

/**
* write corrent bus state to file
*/
void updateBusTrace(MSI_BUS *bus) {
	if (bus->gotNewReq) {
		fprintf(bus->traceFile, "%d %X %X %05X %08X\n",
			bus->clock->cycle, bus->txn.origID.Q, bus->txn.command.Q, bus->txn.address.Q, bus->txn.data.Q);
		bus->gotNewReq = false;
	}
}

/**
* set bus value for next clock cycle
*/
void bus__updateTransaction(MSI_BUS *bus) {
	register_DtoQ((DQ_FF*)(&bus->txn), sizeof(BusTransaction) / sizeof(DQ_FF));
}

/**
* ask bus to set a transaction. will be granted according to arbitration
*/
bool bus__requestTXN(MSI_BUS *bus, OriginatorID origID, BusCommand command, int address, int data, bool override) {
	if (bus->txn.command.D == NO_COMMAND || override) { // bus in not taken, or flush 
		bus->txn.origID.D = origID;
		bus->txn.command.D = command;
		bus->txn.address.D = address;
		bus->txn.data.D = data;
		bus->gotNewReq = true;
		return true;
	}
	return false;
}

/**
* if bus was on flush, next cycle bus is available
*/
void bus__update(MSI_BUS *bus) {
	updateBusTrace(bus);
	if (bus->txn.command.Q == FLUSH) {
		bus->txn.command.D = NO_COMMAND;
	}
}

/**
* init bus struct
*/
void bus__init(MSI_BUS *bus, char *traceFilepath, Clock *clock) {
	bus->clock = clock;
	memset(&bus->txn, 0, sizeof(BusTransaction));
	bus->gotNewReq = false;
	fopen_s(&bus->traceFile, traceFilepath, "w");
}

/**
* save output files on termination
*/
void bus__terminate(MSI_BUS *bus) {
	fclose(bus->traceFile);
}