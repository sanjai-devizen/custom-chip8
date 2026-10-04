<<<<<<< HEAD
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
=======
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
>>>>>>> 7f2fc3e7d6d8871ed1084b5c6190f12de2ff11cf
}