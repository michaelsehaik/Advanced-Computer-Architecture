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

bool doOperation(Cache *cache, int set) {
	if (isLoadOperation(&cache->curOperation)) {
		cache->curOperation.data = cache->DSRAM[set];
	}
	else {
		cache->DSRAM[set] = cache->curOperation.data;
	}
}

bool cache__setNewOperation(Cache *cache, int address, int data, CACHE_OPERATION_NAME opName) { 
	int set = address % CACHE_SIZE;
	int tag = address / CACHE_SIZE;
	
	cache->curOperation.name = opName;
	cache->curOperation.address = address;
	cache->curOperation.data = data;
	bool loadOperation = isLoadOperation(&cache->curOperation);
	//printf("cache %d: setting operation name: %d, addr =%d, data=%d\n", cache->origID, opName, address, data);

	//printf("isLoadOperation=%d\n", isLoadOperation(&cache->curOperation));
	//printf("isBlockInCache=%d\n", isBlockInCache(cache, set, tag));
	//printf("isWriteReady=%d\n", isWriteReady(cache, set, tag));

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

void loadToCache(Cache *cache, int set, int tag) {
	if (cache->curOperation.name == LOAD_WORD) {		
		cache->TSRAM[set].MSIState = SHARED_S;
		//printf("cache %d: SHARED_S set %d, tag %d, value: %d\n", cache->origID, set, tag, cache->TSRAM[set].MSIState);
	}
	else {
		cache->TSRAM[set].MSIState = MODIFIED_S;
		//printf("cache %d: MODIFIED_S set %d, tag %d, value: %d\n", cache->origID, set, tag, cache->TSRAM[set].MSIState);
	}
	cache->TSRAM[set].tag = tag;
	cache->DSRAM[set] = cache->bus->txn.data.Q;
}

void cache__snoop(Cache *cache) {
	int set = cache->bus->txn.address.Q % CACHE_SIZE;
	int tag = cache->bus->txn.address.Q / CACHE_SIZE;
	bool blockInCache = isBlockInCache(cache, set, tag);

	if (cache->bus->txn.origID.Q == cache->origID || !isBlockInCache) return;
	switch (cache->bus->txn.command.Q) {
		case BUS_RD:
			if (isWriteReady(cache, set, tag)) {
				bus__requestTXN(cache->bus, cache->origID, FLUSH, cache->bus->txn.address.Q, cache->DSRAM[set], true); //always granted
				cache->TSRAM[set].MSIState = SHARED_S;
			}
			break;
		case BUS_RDX:
			if (isWriteReady(cache, set, tag)) {
				bus__requestTXN(cache->bus, cache->origID, FLUSH, cache->bus->txn.address.Q, cache->DSRAM[set], true); //always granted
			}
			cache->TSRAM[set].MSIState = INVALID_S;
			break;
		case FLUSH:
			if (cache->bus->txn.address.Q == cache->linkRegister->address && cache->linkRegister->flag) {
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
			flushAddr = getFullAddress(set, cache->TSRAM[set].tag);
			if (bus__requestTXN(cache->bus, cache->origID, FLUSH, flushAddr, cache->DSRAM[set], false)) {
				cache->TSRAM[set].MSIState = SHARED_S;
				cache->state = SEND_READ_S;
			}
			break;
		case SEND_READ_S:
			command = (cache->curOperation.name == LOAD_WORD) ? BUS_RD : BUS_RDX;
			if (bus__requestTXN(cache->bus, cache->origID, command, cache->curOperation.address, 0, false)) {
				cache->state = WAIT_READ_S;
			}
			break;
		case WAIT_READ_S:
			if (cache->bus->txn.command.Q == FLUSH && cache->bus->txn.address.Q == cache->curOperation.address) {
				//printf("set =%x, tag  =%x, address=%x\n", set, tag, cache->curOperation.address);
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
		fprintf(outputFile, "%05X%03X\n", cache->TSRAM[i].MSIState, cache->TSRAM[i].tag);
	}
	fclose(outputFile);
}

void cache__terminate(Cache *cache) {
	createFileFromArray(cache->dsramFilepath, cache->DSRAM, CACHE_SIZE, true, false);
	printTSRAM(cache);
}