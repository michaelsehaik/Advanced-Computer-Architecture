#pragma once

#include <stdbool.h>

/*
	has 1 sram.
	has a register file.
	has 1 pipeline.
	has PC.
	main logic function:
		1. checks (somehow) if still in stall and skips if true. (sram should have a funtion we can question if we are waiting for it)
		2. calls update of each stage (input pipline.Q, output pipeline.D so no dependency on other stages)
		3. update trace
		 
	has status struct to keep track over:
		1. number of clock cycles the core was running till halt
		2. number of instructions executed
*/