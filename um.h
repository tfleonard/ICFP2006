/* 
 * File:   um.h
 * Author: tleonard
 *
 * Created on April 19, 2011, 9:33 AM
 */

#ifndef UM_H
#define	UM_H


#define OPR_MSK		0x7
#define OPRA_SHFT	6
#define OPRB_SHFT	3
#define OPRC_SHFT	0
#define OPRS_SHFT	25
#define LIT_MSK		0x01FFFFFF
#define OPCODE_MSK	0x0000000F
#define OPCODE_SHFT	28

//
// instruction opcodes
//
#define CMOV	0
#define ALD		1
#define ASTR	2
#define ADD		3
#define MUL		4
#define DIV		5
#define NAND	6
#define HALT	7
#define ALLOC	8
#define AFREE	9
#define OUT		10
#define IN		11
#define LDP		12
#define LDI		13












#ifdef	__cplusplus
extern "C" {
#endif

int initUM(void);
int loadFile(char *fname);
int execUM(void);


#ifdef	__cplusplus
}
#endif

#endif	/* UM_H */

