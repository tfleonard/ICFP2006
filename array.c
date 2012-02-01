
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "types.h"
#include "array.h"



ArrayDesc *arrayDescFifo[FIFO_SIZE];		// 1-7
U32 descFifoInPtr = 0;
U32 descFifoOutPtr = 0;
ArrayDesc arrayDesc[NUM_DESC];


//
// initialize the Array Descriptor fifo'
//
int initDescFifo(void) {

int arrayNum = 1;
int i;
ArrayDesc *curDesc;

	curDesc = arrayDesc;
	for (i=0; i<NUM_DESC; i++) {
		curDesc->name = i+1;
		curDesc->size = 0;
		curDesc->adr = 0;
		arrayDescFifo[descFifoInPtr] = curDesc;
		descFifoInPtr = (descFifoInPtr + 1) % FIFO_SIZE;
		curDesc++;
	}
}

int descFifoIsEmpty(void) {

	if (descFifoInPtr == descFifoOutPtr) {
		return 1;
	}
	return 0;
}


ArrayDesc *dequeueDesc(void) {

ArrayDesc *desc;

	if (descFifoIsEmpty()) {
//#ifdef TRACE
fprintf(stderr,"in: %d  out: %d\n",descFifoInPtr,descFifoOutPtr);
//#endif
		return 0;
	}
	desc = arrayDescFifo[descFifoOutPtr];
	descFifoOutPtr = (descFifoOutPtr + 1) % FIFO_SIZE;
	return desc;
}

void enqueueDesc(ArrayDesc *desc) {
	arrayDescFifo[descFifoInPtr] = desc;
	descFifoInPtr = (descFifoInPtr +1) % FIFO_SIZE;
}

ArrayDesc	*getArrayDesc(U32 num) {
	if ( (num > NUM_DESC) || (num < 1) ) {
		return 0;
	}
	return &arrayDesc[num-1];
}

