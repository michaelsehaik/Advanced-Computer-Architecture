#include "core.h"

void initCore() {

}

void doFetchStage() {

}

bool doDecodeStage() {
	return false; // should we stall?
}

void doExecuteStage() {

}

bool doMemStage() {
	// see if we are waiting for bus
	// update pipeline regs if needed
	// call sendRequest(...) method of bus - this does not mean request is granted because of priorities!
	return false; // sould we stall?
}

void doWriteBackStage() {

}

bool executeStep() {
	bool halt = false;
	// if not stalled
		doFetchStage();
	doDecodeStage();	// do we need to stall D+F?
	doExecuteStage();
	doMemStage();		// do we need to stall pipeline?
	doWriteBackStage();
	// writeTraceFile()
	return halt;
}