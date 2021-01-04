#pragma once

#include <stdbool.h>

#include "util.h"

typedef enum PiplineStage {
	FETCH,
	DECODE,
	EXEC,
	MEM,
	WB,
	NUM_OF_STAGES
} PiplineStage;

struct FetchDecoderReg {
	DQ_FF instruction;
	DQ_FF PC;
	DQ_FF valid;
};

struct DecoderExecuteReg {
	DQ_FF A_val;
	DQ_FF B_val;
	DQ_FF rd;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF valid;
};

struct ExecuteMemoryReg {
	DQ_FF aluRes;
	DQ_FF rd;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF valid;
};

struct MemoryWriteBackReg{
	DQ_FF aluRes;
	DQ_FF memValue;
	DQ_FF rd;
	DQ_FF memOpSuccess;
	DQ_FF opcode;
	DQ_FF PC;
	DQ_FF valid;
};

/**
* the pipeline struct, contains registers between stages (FD, DE, EM, MW) + stall information and stats information
*/
typedef struct Pipeline {
	struct FetchDecoderReg IF_ID;
	struct DecoderExecuteReg ID_EX;
	struct ExecuteMemoryReg EX_MEM;
	struct MemoryWriteBackReg MEM_WB;
	bool memStall;
	bool decodeStall;
	int decodeStallCount;
	int memStallCount;
} Pipeline;


void pipeline__init(Pipeline *pipeline);

/**
* update pipeline registers, for each register, move D values to Q values
*/
void pipeline__update(Pipeline* pipeline);