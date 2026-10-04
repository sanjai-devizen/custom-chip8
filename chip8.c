#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
static size_t rom_size = 0;
static int draw_flag = 0;

void load_roms(char *filename, architecture* Arch){
	FILE *file = fopen(filename, "rb");
	if (file == NULL){
		printf("\nError while opening file");
		exit(-1);
	}

	int offset = 0x200; 

	while (fread(Arch->main_memory + offset, 1, 1, file) == 1) {
		
		if (offset >= 4096) {
			printf("\nError: File capacity reached (Max 4096 bytes exceeded)");
			fclose(file);
			exit(-1);
		}

		offset = offset + 1;
		rom_size = rom_size + 1;
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
	if (Arch->PC != 0x200 + rm){
		opcode = (Arch->main_memory[Arch->PC] << 8) | (Arch->main_memory[Arch->PC + 1]);
		printf("\nAddr [0x%03X]: 0x%04X", Arch->PC, opcode);

		switch (opcode & 0xF000){
			case (0x0000):{
				switch (opcode & 0x00FF){
					case (0x00E0):{
						memset(Arch->frame_buffer, 0, sizeof(32 * 64));
						draw_flag = 1;
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x00EE):{
						Arch->PC = Arch->stack[Arch->SP - 1];
						Arch->SP = Arch->SP - 1;
						Arch->PC = Arch->PC + 2;

						break;
					}
					default:{
						printf("\nUnknown opcode !");
						Arch->PC = Arch->PC + 2;

						break;
					}
				}
			}
			case (0x1000):{
				Arch->PC = (opcode & 0x0FFF);

				break;
			}
			case (0x2000):{
				Arch->stack[Arch->SP] = PC;
				Arch->SP = Arch->SP + 1;
				Arch->PC = (opcode & 0x0FFF);

				break;
			} 
			case (0x3000):{
				if (Arch->v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
					Arch->PC = Arch->PC + 4;
				} else {
					Arch->PC = Arch->PC + 2;
				}

				break;
			}
			case (0x4000):{
				if (Arch->v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
					Arch->PC = Arch->PC + 4;
				} else {
					Arch->PC = Arch->PC + 2;
				}

				break;
			}
			case (0x5000):{
				if (Arch->v[(opcode & 0x0F00 >> 8)] == Arch->v[(opcode & 0x00F0 >> 4)]){
					Arch->PC = Arch->PC + 4;
				} else {
					Arch->PC = Arch->PC + 2;
				}

				break;
			}
			case (0x6000):{
				Arch->v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
				Arch->PC = Arch->PC + 2;

				break;
			} 
			case (0x7000):{
				Arch->v[(opcode & 0x0F00) >> 8] = Arch->v[(opcode & 0x0F00) >> 8] + (opcode & 0x00FF);
				Arch->PC = Arch->PC + 2;

				break;
			}
			case (0x8000):{
				switch(opcode & 0x000F){
					case (0x0000):{
						Arch->v[(opcode & 0x0F00) >> 8] = Arch->v[(opcode & 0x00F0) >> 4];
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x0001):{
						Arch->v[(opcode & 0x0F00) >> 8] = Arch->v[(opcode & 0x0F00) >> 8] | Arch->v[(opcode & 0x00F0) >> 4];	
						Arch->PC = Arch->PC + 2;

						break;					
					}
					case (0x0002):{
						Arch->v[(opcode & 0x0F00) >> 8] = Arch->v[(opcode & 0x0F00) >> 8] & Arch->v[(opcode & 0x00F0) >> 4];	
						Arch->PC = Arch->PC + 2;

						break;					
					}
					case (0x0003):{
						Arch->v[(opcode & 0x0F00) >> 8] = Arch->v[(opcode & 0x0F00) >> 8] ^ Arch->v[(opcode & 0x00F0) >> 4];	
						Arch->PC = Arch->PC + 2;

						break;					
					}
					case (0x0004):{
						uint16_t sum = Arch->v[(opcode & 0x0F00) >> 8] + Arch->v[(opcode & 0x00F0) >> 4];
						Arch->v[0xF] = (sum > 0xFF) 0 : 1;
						Arch->v[(opcode & 0x0F00) >> 8] = sum & Arch->v[0xF];
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x0005):{
						
						break;
					}
				}
			}
			case (0x9000):{

			}
			case (0xA000):{

			} 
			case (0xB000):{
				
			}
			case (0xC000):{

			}
			case (0xD000):{

			}
			case (0xE000):{

			} 
			case (0xF000):{
				
			}
		}
	} else {
		printf("\nOut of RAM !");
	}
}

void handle_timers(){

}