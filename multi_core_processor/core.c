#include "core.h"
#include "util.h"

void initCore(Core *core) {

}

void doFetchStage(Core *core) {

}

bool doDecodeStage(Core *core) {
	return false; // should we stall?
}

void doExecuteStage(Core *core) {

}

bool doMemStage(Core *core) {
	// see if we are waiting for bus
	// update pipeline regs if needed
	// call sendRequest(...) method of bus - this does not mean request is granted because of priorities!
	return false; // sould we stall?
}

void doWriteBackStage(Core *core) {

}

bool executeStep(Core *core) {
	bool halt = false;
	// if not stalled
		doFetchStage(core);
	doDecodeStage(core);	// do we need to stall D+F?
	doExecuteStage(core);
	doMemStage(core);		// do we need to stall pipeline?
	doWriteBackStage(core);
	return halt;
}

void updateCoreTrace(Core *core) {
	// TODO
}

bool core__update(Core *core) {
	bool halt;
	sram__update(&core->SRAM);
	halt = executeStep(core);
	updateCoreTrace(core);

	return halt;
}

void core__init(Core *core, MSI_BUS* bus, char *imemFilepath, char *traceFilepath, char *dsramFilepath, char *tsramFilepath, char *statsFilepath) {
	pipeline__init(&core->pipeline);
	sram__init(&core->SRAM, bus, dsramFilepath, tsramFilepath);
	memset(core->registers, 0, REG_FILE_SIZE);
	core->PC = 0;
	core->waitForSRAM;
	core->waitForWB;

	FILE *ImemFile = NULL;
	fopen_s(&ImemFile, imemFilepath, "r");
	memset(core->Imem, 0, IMEM_SIZE*sizeof(int));
	loadArrayFromFile(ImemFile, core->Imem, IMEM_SIZE);
	fclose(ImemFile);
}

void core__terminate(Core *core) {
	sram__terminate(&core->SRAM);
	fclose(core->traceFile);
	fclose(core->statsFile);
}