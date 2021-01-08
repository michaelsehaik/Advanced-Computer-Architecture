#include <stdio.h>

#include "cache.h"

/**
* auxilary functions
*/
bool isBlockInCache(Cache *cache, int set, int tag) {
	return cache->TSRAM[set].MSIState != INVALID_S && tag == cache->TSRAM[set].tag;
}

bool isSetTaken(Cache *cache, int set, int tag) {
	return cache->TSRAM[set].MSIState != INVALID_S && tag != cache->TSRAM[set].tag;
}

bool isLoadOperation(CacheOperation *operation) {
	return (operation->name == LOAD_WORD || operation->name == LOAD_LINKED);
}

int getFullAddress(int set, int tag) {
	return (tag << INDEX_WIDTH) | set;
}

bool isWriteReady(Cache *cache, int set, int tag) {
	return (tag == cache->TSRAM[set].tag && cache->TSRAM[set].MSIState == MODIFIED_S);
}

/**
* return the data to core,set success on SC operations
*/
bool doOperation(Cache *cache, int set) {
	if (isLoadOperation(&cache->curOperation)) {
		cache->curOperation.data = cache->DSRAM[set];
	}
	else {
		cache->DSRAM[set] = cache->curOperation.data;
		if (cache->curOperation.name == STORE_CONDITIONAL) {
			cache->curOperation.data = 1; // success
		}
	}
}

/**
* check if someone wrote to adress on watch
*/
bool checkSCFail(Cache *cache) {
	if (cache->curOperation.name == STORE_CONDITIONAL &&
		!(cache->linkRegister.address == cache->curOperation.address && cache->linkRegister.flag)) { 
		return true; // no need to wait for FSM
	}
	return false;
}

/**
* set watch on address
*/
void setLinkRegister(LinkRegister *reg, int address) {
	reg->address = address;
	reg->flag = true;
}

/**
* handle new memory operation from core
*/
bool cache__setNewOperation(Cache *cache, int address, int data, CACHE_OPERATION_NAME opName) { 
	int set = address % CACHE_SIZE;
	int tag = address / CACHE_SIZE;
	
	cache->curOperation.name = opName;
	cache->curOperation.address = address;
	cache->curOperation.data = data;
	bool loadOperation = isLoadOperation(&cache->curOperation);

	if (checkSCFail(cache)) {
		cache->curOperation.data = 0; // failure
		return false; // no need to wait for FSM
	}

	if ((loadOperation && isBlockInCache(cache, set, tag)) ||
		    (!loadOperation && isWriteReady(cache, set, tag))) {

		if (loadOperation) cache->readHitCount++;
		else cache->writeHitCount++;
		doOperation(cache, set);
		return false;  // no need to wait for FSM
	}

	if (isSetTaken(cache, set, tag) && cache->TSRAM[set].MSIState == MODIFIED_S) {
		cache->state = FLUSH_S;
	}
	else {
		cache->state = SEND_READ_S;
	}
	if (loadOperation) cache->readMissCount++;
	else cache->writeMissCount++;
	return true; // need to wait for FSM
}

/**
* load data to SRAM, and update MSI state
*/
void loadToCache(Cache *cache, int set, int tag) {
	if (isLoadOperation(&cache->curOperation)) {		
		cache->TSRAM[set].MSIState = SHARED_S;
	}
	else {
		cache->TSRAM[set].MSIState = MODIFIED_S;
	}
	cache->TSRAM[set].tag = tag;
	cache->DSRAM[set] = cache->bus->txn.data.Q;
}

/**
* listen to transaction on bus.
*/
void cache__snoop(Cache *cache) {
	int set = cache->bus->txn.address.Q % CACHE_SIZE;
	int tag = cache->bus->txn.address.Q / CACHE_SIZE;
	bool blockInCache = isBlockInCache(cache, set, tag);

	if (cache->bus->txn.origID.Q == cache->origID || !isBlockInCache) return;
	switch (cache->bus->txn.command.Q) {
		case BUS_RD:
			if (isWriteReady(cache, set, tag)) { // data on modified- flush
				bus__requestTXN(cache->bus, cache->origID, FLUSH, cache->bus->txn.address.Q, cache->DSRAM[set], true); //always granted
				cache->TSRAM[set].MSIState = SHARED_S;
			}
			break;
		case BUS_RDX:
			if (isWriteReady(cache, set, tag)) { // data on modified- flush
				bus__requestTXN(cache->bus, cache->origID, FLUSH, cache->bus->txn.address.Q, cache->DSRAM[set], true); //always granted
			}
			if (cache->bus->txn.address.Q == cache->linkRegister.address) { // linked register
				cache->linkRegister.flag = false;
			}
			cache->TSRAM[set].MSIState = INVALID_S;
			break;
	}
}

/**
* handle operation according to cache FSM
*/
void cache__update(Cache *cache) {
	BusTransaction txn;
	BusCommand command;
	int set = cache->curOperation.address % CACHE_SIZE;
	int tag = cache->curOperation.address / CACHE_SIZE;
	int flushAddr;

	switch (cache->state) {
		case FLUSH_S: // empty the taken set
			flushAddr = getFullAddress(set, cache->TSRAM[set].tag);
			if (bus__requestTXN(cache->bus, cache->origID, FLUSH, flushAddr, cache->DSRAM[set], false)) {
				cache->TSRAM[set].MSIState = SHARED_S;
				cache->state = SEND_READ_S;
			}
			break;
		case SEND_READ_S: // send read request
			if (checkSCFail(cache)) {
				cache->curOperation.data = 0; // failure
				cache->state = DATA_READY_S;
			}

			command = (isLoadOperation(&cache->curOperation)) ? BUS_RD : BUS_RDX;
			if (bus__requestTXN(cache->bus, cache->origID, command, cache->curOperation.address, 0, false)) {
				cache->state = WAIT_READ_S;
				if (cache->curOperation.name == LOAD_LINKED) { 
					setLinkRegister(&cache->linkRegister, cache->curOperation.address);
				}
			}
			break;
		case WAIT_READ_S: // wait for bus response
			if (cache->bus->txn.command.Q == FLUSH && cache->bus->txn.address.Q == cache->curOperation.address) {
				loadToCache(cache, set, tag);
				doOperation(cache, set);
				cache->state = DATA_READY_S;
			}
			break;
		case DATA_READY_S:
			cache->state = IDLE_S;
			break;
	}
}

/**
* init cache struct
*/
void cache__init(Cache *cache, MSI_BUS* bus, OriginatorID origID, char *dsramFilepath, char *tsramFilepath) {
	memset(cache->DSRAM, 0, CACHE_SIZE * sizeof(int));
	memset(cache->TSRAM, 0, CACHE_SIZE * sizeof(TSRAM_CELL));
	cache->origID = origID;
	cache->dsramFilepath = dsramFilepath;
	cache->tsramFilepath = tsramFilepath;
	cache->bus = bus;
	cache->state = IDLE_S;
	cache->readHitCount = 0;
	cache->writeHitCount = 0;
	cache->readMissCount = 0;
	cache->writeMissCount = 0;
	cache->linkRegister.flag = false;
}

/**
* print TSRAM to file
*/
void printTSRAM(Cache *cache) {
	FILE *outputFile = NULL;
	fopen_s(&outputFile, cache->tsramFilepath, "w");
	for (int i = 0; i < CACHE_SIZE; i++) {
		fprintf(outputFile, "%05X%03X\n", cache->TSRAM[i].MSIState, cache->TSRAM[i].tag);
	}
	fclose(outputFile);
}

/**
* create output files on termination
*/
void cache__terminate(Cache *cache) {
	createFileFromArray(cache->dsramFilepath, cache->DSRAM, CACHE_SIZE, true, false);
	printTSRAM(cache);
}