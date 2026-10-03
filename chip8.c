#include <stdlib.h>
#include <stdio.h>
#include "defs.h"

uint16_t opcode;
uint8_t keys[16];

uint8_t set_fonts[5 * 16] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
	
static architecture *Arch = NULL;

void load_roms(char *filename, architecture* Arch){
	FILE *file = fopen(filename, "r");
	if (file == NULL){
		printf("\nError while opening file");
		exit(-1);
	}

	unsigned int temp_byte = 0;
	int offset = 0x200; 

	while (fscanf(file, "%2x", &temp_byte) == 1) {
		
		if (offset >= 4096) {
			printf("\nError: File capacity reached (Max 4096 bytes exceeded)");
			fclose(file);
			exit(-1);
		}

		Arch->main_memory[offset] = (uint8_t)temp_byte;
		offset++;
	}

	fclose(file); 
	printf("\nLoaded ROM's into main memory :)");
}


void initialize_chip8(char *filename){
    Arch = (architecture*)malloc(sizeof(architecture));
    if (Arch == NULL) {
        printf("\nError: Architecture memory allocation failed");
        exit(-1);
    }

	Arch->PC = 0x200;
	Arch->SP = 0;
	Arch->IR = 0;
	opcode = 0;

	for(int i = 0; i < 80; i++){
		Arch->main_memory[i] = set_fonts[i];
	}

	Arch->delay_timer = 0;
	Arch->sound_timer = 0;

	printf("\nChip8 hardware and architecture Up and Ready :)");

	load_roms(filename, Arch);
}

void emulate_cycle(){
	opcode = (Arch->main_memory[Arch->PC] << 8) | (Arch->main_memory[Arch->PC + 1]);

	printf("\nAddr [0x%03X]: 0x%04X", Arch->PC, opcode);

	switch (opcode & 0x0F000){
		
	}
}

void handle_timers(){

}
