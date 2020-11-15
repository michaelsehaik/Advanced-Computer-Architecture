#include "pipeline.h"

void pipeline__init(Pipeline *pipeline) {
	for (int i = 0; i < NUM_OF_PIPELINE_REGS; i++) {
		pipeline->D[i].valid = false;
		pipeline->Q[i].valid = false;
	}
}