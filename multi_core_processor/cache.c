#include <stdio.h>

#include "cache.h"

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
	return (isBlockInCache(cache, set, tag) && cache->TSRAM[set].MSIState == MODIFIED_S);
}

void cache__setNewOperation(Cache *cache, int address, int data, CACHE_OPERATION_NAME opName) { 
	int set = address % CACHE_SIZE;
	int tag = address / CACHE_SIZE;

	printf("setting operation name: %d\n", opName);
	cache->curOperation.name = opName;
	cache->curOperation.address = address;
	cache->curOperation.data = data;

	if ((isLoadOperation(&cache->curOperation) && isBlockInCache(cache, set, tag)) ||
		    (!isLoadOperation(&cache->curOperation) && isWriteReady(cache, set, tag))) {
		cache->state = DATA_READY_S;
	}
	else if (isSetTaken(cache, set, tag)) {
		cache->state = FLUSH_S;
	}
	else {
		cache->state = SEND_READ_S;
	}
}

void loadToCache(Cache *cache, int set, int tag) {
	if (cache->curOperation.name == LOAD_WORD) {		
		cache->TSRAM[set].MSIState = SHARED_S;
	}
	else {
		cache->TSRAM[set].MSIState = MODIFIED_S;
	}
	cache->TSRAM[set].tag = tag;
	cache->DSRAM[set] = cache->bus->txn.data;
}

void cache__snoop(Cache *cache) {
	BusTransaction txn;
	int set = cache->bus->txn.address % CACHE_SIZE;
	int tag = cache->bus->txn.address / CACHE_SIZE;
	bool blockInCache = isBlockInCache(cache, set, tag);

	if (cache->bus->txn.origID == cache->origID || !isBlockInCache) return;
	switch (cache->bus->txn.command) {
		case BUS_RD:
			if (cache->TSRAM[set].MSIState == MODIFIED_S) {
				bus__createTransaction(&txn, cache->origID, FLUSH, cache->bus->txn.address, cache->DSRAM[set]);
				bus__setTransaction(cache->bus, txn);
				cache->TSRAM[set].MSIState = SHARED_S;
			}
			break;
		case BUS_RDX:
			if (cache->TSRAM[set].MSIState == MODIFIED_S) { // FIXME: need bus D and Q.. add a stage Q=D after snoop or not all cores will invalidate
				bus__createTransaction(&txn, cache->origID, FLUSH, cache->bus->txn.address, cache->DSRAM[set]);
				bus__setTransaction(cache->bus, txn);
			}
			cache->TSRAM[set].MSIState = INVALID_S;
			break;
		case FLUSH:
			if (cache->bus->txn.address == cache->linkRegister->address && cache->linkRegister->flag) {
				cache->linkRegister->flag = false;
			}
			break;
	}
}

void cache__update(Cache *cache) {
	BusTransaction txn;
	BusCommand command;
	int set = cache->curOperation.address % CACHE_SIZE;
	int tag = cache->curOperation.address / CACHE_SIZE;
	int flushAddr;

	switch (cache->state) {
		case FLUSH_S:
			flushAddr = getFullAddress(cache->TSRAM[set].tag, set);
			bus__createTransaction(&txn, cache->origID, FLUSH_S, flushAddr, cache->DSRAM[set]);
			if (bus__requestTXN(cache->bus, txn)) {
				cache->state = SEND_READ_S;
			}
			break;
		case SEND_READ_S:
			command = (cache->curOperation.name == LOAD_WORD) ? BUS_RD : BUS_RDX;
			bus__createTransaction(&txn, cache->origID, command, cache->curOperation.address, cache->DSRAM[set]);
			if (bus__requestTXN(cache->bus, txn)) {
				cache->state = WAIT_READ_S;
			}
			break;
		case WAIT_READ_S:
			if (cache->bus->txn.command == FLUSH && cache->bus->txn.address == cache->curOperation.address) {
				loadToCache(cache, set, tag);
				cache->state = DATA_READY_S;
			}
			break;
		case DATA_READY_S:
			if (isLoadOperation(&cache->curOperation)) {
				cache->curOperation.data = cache->DSRAM[set];
			}
			else {
				cache->DSRAM[set] = cache->curOperation.data;
			}
			cache->state = DONE_S;
			break;
		case DONE_S:
			cache->state = IDLE_S;
			break;
	}
}

void cache__init(Cache *cache, MSI_BUS* bus, OriginatorID origID, char *dsramFilepath, char *tsramFilepath, RegisterDMA *linkRegister) {
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
	cache->linkRegister = linkRegister;
}

void printTSRAM(Cache *cache) {
	FILE *outputFile = NULL;
	fopen_s(&outputFile, cache->tsramFilepath, "w");
	for (int i = 0; i < CACHE_SIZE; i++) {
		fprintf(outputFile, "%05X%03X\n", cache->TSRAM->MSIState, cache->TSRAM->tag);
	}
	fclose(outputFile);
}

void cache__terminate(Cache *cache) {
	createFileFromArray(cache->dsramFilepath, cache->DSRAM, CACHE_SIZE, true);
	printTSRAM(cache);
}