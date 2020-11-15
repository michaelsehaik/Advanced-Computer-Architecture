#pragma once

typedef struct DRAM{
	int x;
	MSI_BUS *bus;
} DRAM;

/*
	just a simple array for dram. Of course it should have init and print log functions.
	need to be able to handle requests form bus
*/