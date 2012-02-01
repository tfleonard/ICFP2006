/* 
 * File:   array.h
 * Author: tleonard
 *
 * Created on April 18, 2011, 9:27 PM
 */

#ifndef ARRAY_H
#define	ARRAY_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
	u32 name;
	u32 size;
	u32 *adr;
} ArrayDesc;


#define NUM_DESC 0x00800000
#define FIFO_SIZE (NUM_DESC+1)
#define FIRST_ARRAY_NUM	0


int initDescFifo(void);
int descFifoIsEmpty(void);
ArrayDesc *dequeueDesc(void);
void enqueueDesc(ArrayDesc *desc);
ArrayDesc	*getArrayDesc(U32 num);







#ifdef	__cplusplus
}
#endif

#endif	/* UM_H */

