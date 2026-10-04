#include <stdio.h>
#include "defs.h"

int
main(int argc, char *argv[]){
	initialize_chip8(argv[1]);

	int run_next_inst = 1;

	while(run_next_inst){
		emulate_cycle();

		printf("\nShow next instruction ? ");
		scanf("%d", &run_next_inst);
	}

	return 0;
}