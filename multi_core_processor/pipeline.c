#include "pipeline.h"

void pipeline__init(Pipeline *pipeline) {
	//for (int i = 0; i < NUM_OF_PIPELINE_REGS; i++) {
	//	pipeline->D[i].valid = false;
	//	pipeline->Q[i].valid = false;
	//}
}

void FF_DtoQ(DQ_FF* FF) {
	FF->Q = FF->D;
}

void pipeline__update(Pipeline* pipeline) {
	
}