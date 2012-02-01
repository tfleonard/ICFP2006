#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "types.h"
#include "array.h"
#include "um.h"



//
// uncomment the next line to enable tracing
//
//#define TRACE 1


//
// machine registers
U32	regFile[8];
U32 pc;
U32 *memPc;
//
// code array is array 0
//
ArrayDesc array0;			// code space




int initUM(void) {

U32 i;
//
	// initialize code array
	//
	array0.adr = 0;
	array0.name = 0;
	array0.size = 0;
	//
	// initialize array pool
	//
	initDescFifo();
	//
	// initialize registers
	//
	pc = 0;
	for (i=0; i < 8; i++) {
		regFile[i] = 0;
	}
}


int loadFile(char *fname) {

	FILE *fp;
	U32 fsizeBytes;
	U32 fsizeWords;
	U32 result;
U32 cnt;
union {
	U32 w;
	U8 b[4];
} btw;
int i;

	fp = fopen(fname,"rb");
	if (!fp) {
		printf(" unable to open source file %s\n", fname);
		return 0;
	}
	fseek(fp,0,SEEK_END);
	fsizeBytes = ftell(fp);
	fsizeWords = fsizeBytes/4;
	printf(" file %s size; %d\n", fname, fsizeBytes);
	fseek(fp,0,SEEK_SET);
	array0.adr = (U32 *)malloc(fsizeBytes);
	if (!array0.adr) {
		printf("unable to allocate %d bytes for program %s\n", fsizeBytes, fname);
		fclose(fp);
		return 0;
	}
	array0.size = fsizeWords;			// number of words

//	result = fread(array0.adr, 4, fsizeWords, fp);

	//
	// the instructions are big endian, we need to convert to little endian
	//
	cnt = 0;
	while (cnt < fsizeWords) {
		for (i=0; i<4; i++) {
			result = fread(&btw.b[3-i],1,1,fp);
			if (!result) {
				break;
			}
		}
		if (!result) {
			break;
		}
		array0.adr[cnt] = btw.w;
		cnt++;
	}

	fclose(fp);
	if (cnt != fsizeWords) {
		printf(" read %d words, expected %d words\n",cnt, fsizeWords);
		free(array0.adr);
		return 0;
	}
}


//
// um main loop
//
int execUM(void) {

U32 inst;
U32 halt = 0;
U32 *progBase;
U32 opa;
U32 opb;
U32 opc;
U32 ops;

	progBase = array0.adr;
	pc = 0;

	while (!halt) {
		if (pc > array0.size) {
			printf(" pc out of range : %08x\n", pc);
			halt = 1;
		}

		inst = progBase[pc];
		pc++;
		opa = (inst >> OPRA_SHFT) & OPR_MSK;
		opb = (inst >> OPRB_SHFT) & OPR_MSK;
		opc = (inst >> OPRC_SHFT) & OPR_MSK;
		ops = (inst >> OPRS_SHFT) & OPR_MSK;

		switch ((inst >> OPCODE_SHFT) & OPCODE_MSK) {

			case CMOV:
#ifdef TRACE
fprintf(stderr,"%08x  %08x  CMOV R%d R%d R%d       ; ", pc-1, inst, opa, opb, opc);
#endif
				if (regFile[opc]) {
					regFile[opa]=regFile[opb];
#ifdef TRACE
fprintf(stderr,"R%d = %08x\n", opa, regFile[opa]);
#endif
				} else {
#ifdef TRACE
fprintf(stderr,"NOP\n");
#endif
				}
				break;

			case ALD:{

				ArrayDesc *array;
#ifdef TRACE
fprintf(stderr,"%08x  %08x  ALD R%d R%d [R%d]       ; array: %08x  R%d = ", pc-1, inst, opa, opb, opc,
		regFile[opb],opa);
#endif
				if (!regFile[opb]) {
					regFile[opa] = progBase[regFile[opc]];
#ifdef TRACE
fprintf (stderr,"%08x\n", regFile[opa]);
#endif
				} else {

					array = getArrayDesc(regFile[opb]);
					if (!array) {
						printf("illegal array access: %08x\n",regFile[opb] );
						halt = 1;
					} else {
						U32 *abase = array->adr;
						regFile[opa] = abase[regFile[opc]];
#ifdef TRACE
fprintf (stderr,"%08x\n", regFile[opa]);
#endif
					}

				}
				break;
			}

			case ASTR: {

				ArrayDesc *array;
#ifdef TRACE
fprintf(stderr,"%08x  %08x  ASTR R%d [R%d] R%d       ; array: %08x  offset: %08x = %08x\n", pc-1, inst, opa, opb, opc,
		regFile[opa],regFile[opb],regFile[opc]);
#endif
				if (!regFile[opa]) {
					progBase[regFile[opb]] = regFile[opc];

				} else {
					array = getArrayDesc(regFile[opa]);
					if (!array) {
						printf("illegal array access: %08x\n",regFile[opa] );
						halt = 1;
					} else {
						U32 *abase = array->adr;
						abase[regFile[opb]] = regFile[opc];
					}

				}
				break;
			}

			case ADD:
#ifdef TRACE
fprintf(stderr,"%08x  %08x  ADD R%d R%d R%d       ; R%d=%08x + R%d=%08x\n", pc-1, inst, opa, opb, opc,
		opb,regFile[opb],opc,regFile[opc]);
#endif
				regFile[opa] = regFile[opb] + regFile[opc];
				break;

			case MUL:
#ifdef TRACE
fprintf(stderr,"%08x  %08x  MUL R%d R%d R%d       ; R%d=%08x * R%d=%08x\n", pc-1, inst, opa, opb, opc,
		opb,regFile[opb],opc,regFile[opc]);
#endif
				regFile[opa] = regFile[opb] * regFile[opc];
				break;

			case DIV:
#ifdef TRACE
fprintf(stderr,"%08x  %08x  DIV R%d R%d R%d       ; R%d=%08x / R%d=%08x\n", pc-1, inst, opa, opb, opc,
		opb,regFile[opb],opc,regFile[opc]);
#endif
				if (!regFile[opc]) {
					printf(" divide by zero\n");
					halt = 1;
				} else {
					regFile[opa] = regFile[opb] / regFile[opc];
				}
				break;

			case NAND: {

				U32 tempb = ~regFile[opb];
				U32 tempc = ~regFile[opc];
#ifdef TRACE
fprintf(stderr,"%08x  %08x  NAND R%d R%d R%d       ; R%d=%08x =  R%d=%08x NAND R%d=%08x\n", pc-1, inst, opa, opb, opc,
		opa,tempb | tempc,opb,regFile[opb],opc,regFile[opc]);
#endif
				regFile[opa] = tempb | tempc;
				break;
			}

			case HALT:
#ifdef TRACE
fprintf(stderr,"%08x  %08x  HALT\n");
#endif
				halt = 1;
				printf(" programmed halt encounterd at %08x\n", pc);
				break;

			case ALLOC: {

				ArrayDesc *array = dequeueDesc();
#ifdef TRACE
fprintf(stderr,"%08x  %08x  ALLOC R%d R%d       ; R%d=%08x ", pc-1, inst, opb, opc,
		opc,regFile[opc]);
#endif
				if (!array) {
					printf(" no array descriptors left\n");
					halt = 1;

				} else {
					U32 size = regFile[opc];

					regFile[opb] = array->name;
					array->adr = (U32 *)malloc(size*4);
					array->size = size;
					if (!array->adr) {
						printf("unable to allocate %08x words for array %d\n", size, array->name);
						halt = 1;
					} else {
						memset(array->adr,0,size*4);
					}
#ifdef TRACE
fprintf(stderr,"array=%08x\n",regFile[opb]);
#endif
				}
				break;
			}

			case AFREE: {

				ArrayDesc *array = getArrayDesc(regFile[opc]);
#ifdef TRACE
fprintf(stderr,"%08x  %08x  AFREE R%d       ; R%d=%08x\n", pc-1, inst, opc,opc,regFile[opc]);
#endif
				if (array->adr) {
					free(array->adr);
				}
				array->size = 0;
				array->adr = 0;
				enqueueDesc(array);
				break;
			}

			case OUT: {

				int c = regFile[opc];
#ifdef TRACE
fprintf(stderr,"%08x  %08x  OUT R%d\n", pc-1, inst, opc);
#endif
				if ( (c >=0) && (c < 256)) {
					putchar(c);
				}
				break;
			}

			case IN: {

				int c = getchar();
#ifdef TRACE
fprintf(stderr,"%08x  %08x  IN R%d\n", pc-1, inst, opc);
#endif
				if (c == EOF) {
					regFile[opc] = 0xFFFFFFFF;
				} else {
					regFile[opc] = c;
				}
				break;
			}

			case LDP: {

				ArrayDesc *array;
#ifdef TRACE
fprintf(stderr,"%08x  %08x  LDP R%d R%d        ; array=%08x  pc= %08x\n", pc-1, inst, opb, opc,regFile[opb],regFile[opc]);
#endif
				if (regFile[opb] == 0) {
					pc = regFile[opc];

				} else {

					array = getArrayDesc(regFile[opb]);

					if (array0.adr) {
						free(array0.adr);
					}
					array0.size = array->size;
					if (array0.size) {
						array0.adr = malloc(array0.size * 4);
						memcpy(array0.adr,array->adr,array0.size*4);
						progBase = array0.adr;
						pc = regFile[opc];
					} else {
						printf(" attempt to clone array %d failed: array size is 0\n", array->name);
						halt = 1;
					}
				}
				break;
			}

			case LDI:
#ifdef TRACE
fprintf(stderr,"%08x  %08x  LDI R%d %08x\n", pc-1, inst, ops, inst & LIT_MSK);
#endif
				regFile[ops] = inst & LIT_MSK;
				break;

			default:
				fprintf(stderr,"%08x  %08x  ILLEGAL INSTRUCTION\n", pc-1, inst);
				halt = 1;

		}
	}
}







