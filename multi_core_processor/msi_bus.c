#include "msi_bus.h"

void updateBusTrace(MSI_BUS *bus) {
	if (bus->gotNewReq) {
		fprintf(bus->traceFile, "%d %X %X %05X %08X\n",
			bus->clock->cycle, bus->txn.origID.Q, bus->txn.command.Q, bus->txn.address.Q, bus->txn.data.Q);
		bus->gotNewReq = false;
	}
}

void bus__updateTransaction(MSI_BUS *bus) {
	register_DtoQ((DQ_FF*)(&bus->txn), sizeof(BusTransaction) / sizeof(DQ_FF));
	//printf("BUS: TXN, id.D: %d, command.D: %d\n", bus->txn.origID.D, bus->txn.command.D);
	//printf("BUS: TXN, id.Q: %d, command.Q: %d\n", bus->txn.origID.Q, bus->txn.command.Q);
}

bool bus__requestTXN(MSI_BUS *bus, OriginatorID origID, BusCommand command, int address, int data, bool override) {
	if (bus->txn.command.D == NO_COMMAND || override) { // bus in not taken, or flush 
		printf("BUS: recieved TXN, id: %d, command: %d\n", origID, command);
		bus->txn.origID.D = origID;
		bus->txn.command.D = command;
		bus->txn.address.D = address;
		bus->txn.data.D = data;
		bus->gotNewReq = true;
		return true;
	}
	return false;
}

void bus__update(MSI_BUS *bus) {
	updateBusTrace(bus);
	if (bus->txn.command.Q == FLUSH) {
		bus->txn.command.D = NO_COMMAND;
	}
}

void bus__init(MSI_BUS *bus, char *traceFilepath, Clock *clock) {
	bus->clock = clock;
	memset(&bus->txn, 0, sizeof(BusTransaction));
	bus->gotNewReq = false;
	fopen_s(&bus->traceFile, traceFilepath, "w");
}

void bus__terminate(MSI_BUS *bus) {
	fclose(bus->traceFile);
}