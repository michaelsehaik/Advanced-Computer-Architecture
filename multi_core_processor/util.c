#include "util.h"

void FF_DtoQ(DQ_FF* FF) {
	FF->Q = FF->D;
}

void register_DtoQ(DQ_FF* FFs, int numOfFF) {
	for (int i = 0; i < numOfFF; i++) {
		FF_DtoQ(&FFs[i]);
	}
}