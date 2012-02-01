/* 
 * File:   main.c
 * Author: tleonard
 *
 * Created on April 18, 2011, 9:26 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "types.h"

#include "um.h"
#include "array.h"


int main(int argc, char** argv) {

	if (argc < 2) {
		printf("usage:  icfp2006 file");
		exit(1);
	}
	initUM();
	loadFile(argv[1]);
	execUM();

	return (EXIT_SUCCESS);
}

