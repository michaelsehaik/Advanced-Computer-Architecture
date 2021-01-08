#pragma once

#include <stdio.h>
#include <stddef.h>

#include "msi_bus.h"
#include "util.h"
#include "IO.h"

#define CACHE_SIZE 256
#define INDEX_WIDTH 8
#define TAG_WIDTH 12 
#define MSI_WIDTH 2

enum MSI_STATE {
	INVALID_S,
	SHARED_S,
	MODIFIED_S
};

typedef enum CACHE_OPERATION_NAME{ 
	LOAD_WORD,
	LOAD_LINKED,
	STORE_WORD,
	STORE_CONDITIONAL
} CACHE_OPERATION_NAME;

typedef struct CacheOperation {
	CACHE_OPERATION_NAME name;
	int data;
	int address;
} CacheOperation;

typedef enum CACHE_STATE { // used by cache FSM
	IDLE_S,
	FLUSH_S,
	SEND_READ_S,
	WAIT_READ_S,
	DATA_READY_S
} CACHE_STATE;

typedef struct TSRAM_CELL { // (unsigned is important for bit fields)
	unsigned short tag : TAG_WIDTH;
	unsigned short MSIState : MSI_WIDTH;
} TSRAM_CELL;

typedef struct Cache {
	MSI_BUS* bus;
	int DSRAM[CACHE_SIZE];
	TSRAM_CELL TSRAM[CACHE_SIZE];
	char *dsramFilepath;
	char *tsramFilepath;
	OriginatorID origID;
	CacheOperation curOperation;
	CACHE_STATE state;
	LinkRegister linkRegister;
	int readHitCount;
	int writeHitCount;
	int readMissCount;
	int writeMissCount;
} Cache;

/**
* handle new memory operation from core
*/
bool cache__setNewOperation(Cache *cache, int address, int data, CACHE_OPERATION_NAME opName);
/**
* listen to transaction on bus.
*/
void cache__snoop(Cache *cache);
/**
* init cache struct
*/
void cache__init(Cache *cache, MSI_BUS* bus, OriginatorID origID, char *dsramFilepath, char *tsramFilepath);
/**
* handle operation according to cache FSM
*/
void cache__update(Cache *cache);
/**
* create output files on termination
*/
void cache__terminate(Cache *cache);