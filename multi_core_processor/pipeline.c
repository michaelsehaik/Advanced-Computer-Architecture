#include "pipeline.h"

void pipeline__init(Pipeline *pipeline) {
	memset(pipeline, 0, sizeof(Pipeline));
}

void pipeline__update(Pipeline* pipeline) {
	if(!(pipeline->decodeStall || pipeline->memStall))
		register_DtoQ((DQ_FF*)(&pipeline->IF_ID), sizeof(struct FetchDecoderReg)/sizeof(DQ_FF));
	if (!(pipeline->memStall))
		register_DtoQ((DQ_FF*)(&pipeline->ID_EX), sizeof(struct DecoderExecuteReg) / sizeof(DQ_FF));
	if (!pipeline->memStall)
		register_DtoQ((DQ_FF*)(&pipeline->EX_MEM), sizeof(struct ExecuteMemoryReg) / sizeof(DQ_FF));
	register_DtoQ((DQ_FF*)(&pipeline->MEM_WB), sizeof(struct MemoryWriteBackReg) / sizeof(DQ_FF));
}