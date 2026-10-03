#include <stdint.h>

#ifndef CHIP8_H
#define CHIP8_H

typedef struct {
	uint8_t main_memory[4096];
	uint8_t v[16];
	uint16_t IR;
	uint16_t stack[64];
	uint16_t SP;
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t frame_buffer[32 * 64];
	uint16_t PC;
} architecture;

void initialize_chip8(char *filename);
void emulate_cycle();

#endif 