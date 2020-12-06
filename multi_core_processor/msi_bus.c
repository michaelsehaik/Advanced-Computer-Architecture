#include "msi_bus.h"

void updateBusTrace(MSI_BUS *bus) {
	if (bus->gotNewReq) {
		fprintf(bus->traceFile, "%d %X %X %05X %08X\n",
			bus->clock->cycle, bus->txn.origID, bus->txn.command, bus->txn.address, bus->txn.data);
		bus->gotNewReq = false;
	}
}

void bus__createTransaction(BusTransaction *txn, OriginatorID origID, BusCommand command, int address, int data) {
	txn->origID = origID;
	txn->command = command;
	txn->address = address;
	txn->data = data;
}

void bus__setTransaction(MSI_BUS *bus, BusTransaction txn) {
	bus->txn = txn;
	bus->gotNewReq = true;
}

bool bus__requestTXN(MSI_BUS *bus, BusTransaction txn) {
	if (bus->txn.command == NO_COMMAND) {
		bus->txn = txn;
		bus->gotNewReq = true;
		return true;
	}
	return false;
}

void bus__update(MSI_BUS *bus) {
	updateBusTrace(bus);
	if (bus->txn.command == FLUSH) {
		bus->txn.command = NO_COMMAND;
	}
}

void bus__init(MSI_BUS *bus, char *traceFilepath, Clock *clock) {
	bus->clock = clock;
	bus->txn.command = NO_COMMAND;
	bus->gotNewReq = false;
	fopen_s(&bus->traceFile, traceFilepath, "w");
}

void bus__terminate(MSI_BUS *bus) {
	fclose(bus->traceFile);
}