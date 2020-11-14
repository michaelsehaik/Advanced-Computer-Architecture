#pragma once

/*
	struct for bus which is shared by srams and dram (see some fields in pdf).
	has methods for trace, sram/dram send request but they are not always valuated.
	the struct updates after we simulated sram+dram for a clock cycle. 
*/