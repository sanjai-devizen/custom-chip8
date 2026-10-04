#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
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
	if (Arch->PC != 0x200 + rom_size){
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
				Arch->stack[Arch->SP] = Arch->PC;
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
						Arch->v[0xF] = (sum > 0xFF) ? 0 : 1;
						Arch->v[(opcode & 0x0F00) >> 8] = sum & Arch->v[0xF];
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x0005):{
						uint16_t diff = Arch->v[(opcode & 0x0F00) >> 8] - Arch->v[(opcode & 0x00F0) >> 4];
						Arch->v[0xF] = (diff < 0) ? 0 : 1;
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x0006):{
						Arch->v[0xF] = Arch->v[(opcode & 0x0F00) >> 8] & 1;
						Arch->v[(opcode & 0x0F00) >> 8] = Arch->v[(opcode & 0x0F00) >> 8] >> 1;
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x0007):{
						uint16_t diff = Arch->v[(opcode & 0x00F0) >> 4] - Arch->v[(opcode & 0x0F00) >> 8];	
						Arch->v[0xF] = (diff < 0) ? 0 : 1;	
						Arch->PC = Arch->PC + 2;
							
						break;							
					}
					case (0x000E):{
						Arch->v[0xF] = Arch->v[(opcode & 0x0F00) >> 8] >> 7;
						Arch->v[(opcode & 0x0F00) >> 8] = Arch->v[(opcode & 0x0F00) >> 8] << 1;
						Arch->PC = Arch->PC + 2;	
						
						break;					
					}
				}
			}
			case (0x9000):{
				if ((opcode & 0x000F) == 0x0000){
					if (Arch->v[(opcode & 0x0F00 >> 8)] != Arch->v[(opcode & 0x00F0 >> 4)]){
						Arch->PC = Arch->PC + 4;
					} else {
						Arch->PC = Arch->PC + 2;
					}

					break;				
				} else {
					printf("\nUnknown opcode !");
				}

				break;
			}
			case (0xA000):{
				Arch->IR = (opcode & 0x0FFF);
				Arch->PC = Arch->PC + 2; 

				break;
			} 
			case (0xB000):{
				Arch->PC = (opcode & 0x0FFF) + Arch->v[0x0];

				break;
			}
			case (0xC000):{
				srand(time(NULL));

				uint8_t random_byte = rand() & 0xFF;
				Arch->v[(opcode & 0x0F00) >> 8] = Arch->v[(opcode & 0x0F00) >> 8] & random_byte;
				Arch->PC = Arch->PC + 2;

				break;
			}
			case (0xD000):{
				uint8_t x = Arch->v[(opcode & 0x0F00) >> 8], y = Arch->v[(opcode & 0x00F0) >> 4];
				uint8_t height = (opcode & 0x000F);
				uint8_t pixel;				

				Arch->v[0xF] = 0;

				for(int sprite_row = 0; sprite_row < height; sprite_row++){
					pixel = Arch->main_memory[Arch->IR + sprite_row];

					for(int sprite_col = 0; sprite_col < 8; sprite_col++){
						if (pixel & (0x80 >> sprite_col) != 0){
							int in_screen_x = (x & 63) + sprite_col;
							int in_screen_y = (y & 31) + sprite_row;

							if (in_screen_x >= 64 || in_screen_y >= 32) continue;

							int in_screen_pos = in_screen_x + (in_screen_y * 64);

							if (Arch->frame_buffer[in_screen_pos] != 0) Arch->v[0xF] = 1;

							Arch->frame_buffer[in_screen_pos] = Arch->frame_buffer[in_screen_pos] ^ 1;
						}
					}
				}
				draw_flag = 1;
				Arch->PC = Arch->PC + 2;

				break;
			}
			case (0xE000):{
				switch (opcode & 0x00FF){
					case (0x009E):{
						if (keys[Arch->v[(opcode & 0x0F00) >> 8]] != 0){
							Arch->PC = Arch->PC + 4;
						} else {
							Arch->PC = Arch->PC + 2;
						}

						break;
					}	
					case (0x00A1):{
						if (keys[Arch->v[(opcode & 0x0F00) >> 8]] == 0){
							Arch->PC = Arch->PC + 4;
						} else {
							Arch->PC = Arch->PC + 2;
						}

						break;
					}
					default:{
						printf("\nUnknown opcode !");

						break;
					}
				}
			} 
			case (0xF000):{
				switch (opcode & 0x00FF){
					case (0x0007):{
						Arch->v[(opcode & 0x0F00) >> 8] = Arch->delay_timer;
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x000A):{
						for(int i = 0; i < 16; i++0){
							if (keys[i] != 0){
								Arch->v[(opcode & 0x0F00) >> 8] = keys[i];

								break;
							}
						}
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x0015):{
						Arch->delay_timer = Arch->v[(opcode & 0x0F00) >> 8];
						Arch->PC = Arch->PC + 2;

						break;						
					}
					case (0x0018):{
						Arch->sound_timer = Arch->v[(opcode & 0x0F00) >> 8];
						Arch->PC = Arch->PC + 2;

						break;										
					}
					case (0x001E):{
						Arch->IR = Arch->IR + Arch->v[(opcode & 0x0F00) >> 8];		
						Arch->PC = Arch->PC + 2;

						break;	
					}
					case (0x0029):{
						Arch->IR = Arch->v[(opcode & 0x0F00) >> 8] * 5;
						Arch->PC = Arch->PC + 2;

						break;		
					}
					case (0x0033):{
						uint8_t bin_to_dec = Arch->v[(opcode & 0x0F00) >> 8];
						Arch->main_memory[Arch->IR] = bin_to_dec / 100;
						Arch->main_memory[Arch->IR + 1] = (bin_to_dec / 10) % 10; 
 						Arch->main_memory[Arch->IR + 2]	= bin_to_dec % 10;
 						Arch->PC = Arch->PC + 2;

 						break;
					}
					case (0x0055):{
						for(int i = 0; i < (opcode & 0x0F00); i++){
							Arch->main_memory[Arch->IR + i] = Arch->v[i];
						}
						Arch->PC = Arch->PC + 2;

						break;
					}
					case (0x0065):{
						for(int i = 0; i < (opcode & 0x0F00); i++){
							Arch->v[i] = Arch->main_memory[Arch->IR + i];
						}
						Arch->PC = Arch->PC + 2;

						break;						
					}
					default:{
						printf("\nUnknown opcode !");

						break;
					}
				}
			}
		}
	} else {
		printf("\nOut of RAM !");
	}
}

void handle_timers(){
	if (Arch->delay_timer > 0){
		Arch->delay_timer = Arch->delay_timer - 1;
	}
	if (Arch->sound_timer > 1){
		Arch->sound_timer = Arch->sound_timer - 1;
	} else {
		printf("\nBEEP !");
	}
}