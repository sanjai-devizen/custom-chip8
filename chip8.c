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

architecture *Arch = NULL;
size_t rom_size = 0;
int draw_flag = 0;

void load_roms(char *filename, architecture* Arch) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
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
        offset++;
        rom_size++;
    }

    fclose(file); 
    printf("\nLoaded ROM into main memory :)");
}

void initialize_chip8(char *filename) {
    srand((unsigned int)time(NULL));

    Arch = (architecture*)malloc(sizeof(architecture));
    if (Arch == NULL) {
        printf("\nError: Architecture memory allocation failed");
        exit(-1);
    }

    memset(Arch->main_memory, 0, sizeof(Arch->main_memory));
    memset(Arch->v, 0, sizeof(Arch->v));
    memset(Arch->stack, 0, sizeof(Arch->stack));
    memset(Arch->frame_buffer, 0, sizeof(Arch->frame_buffer));
    memset(keys, 0, sizeof(keys));

    Arch->PC = 0x200;
    Arch->SP = 0;
    Arch->IR = 0;
    opcode = 0;

    for (int i = 0; i < 80; i++) {
        Arch->main_memory[i] = set_fonts[i];
    }

    Arch->delay_timer = 0;
    Arch->sound_timer = 0;

    printf("\nChip8 hardware and architecture Up and Ready :)");
    load_roms(filename, Arch);
}

void emulate_cycle(void) {
    if (Arch->PC < 4096 - 1) {
        opcode = (Arch->main_memory[Arch->PC] << 8) | (Arch->main_memory[Arch->PC + 1]);

        switch (opcode & 0xF000) {
            case 0x0000: {
                switch (opcode & 0x00FF) {
                    case 0x00E0: {
                        memset(Arch->frame_buffer, 0, 64 * 32);
                        draw_flag = 1;
                        Arch->PC += 2;
                        break;
                    }
                    case 0x00EE: {
                        if (Arch->SP > 0) {
                            Arch->SP--;
                            Arch->PC = Arch->stack[Arch->SP];
                        }
                        Arch->PC += 2;
                        break;
                    }
                    default:
                        Arch->PC += 2;
                        break;
                }
                break;
            }
            case 0x1000: {
                Arch->PC = (opcode & 0x0FFF);
                break;
            }
            case 0x2000: {
                Arch->stack[Arch->SP] = Arch->PC;
                Arch->SP++;
                Arch->PC = (opcode & 0x0FFF);
                break;
            }
            case 0x3000: {
                if (Arch->v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                    Arch->PC += 4;
                } else {
                    Arch->PC += 2;
                }
                break;
            }
            case 0x4000: {
                if (Arch->v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                    Arch->PC += 4;
                } else {
                    Arch->PC += 2;
                }
                break;
            }
            case 0x5000: {
                if (Arch->v[(opcode & 0x0F00) >> 8] == Arch->v[(opcode & 0x00F0) >> 4]) {
                    Arch->PC += 4;
                } else {
                    Arch->PC += 2;
                }
                break;
            }
            case 0x6000: {
                Arch->v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
                Arch->PC += 2;
                break;
            }
            case 0x7000: {
                Arch->v[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
                Arch->PC += 2;
                break;
            }
            case 0x8000: {
                uint8_t x = (opcode & 0x0F00) >> 8;
                uint8_t y = (opcode & 0x00F0) >> 4;

                switch (opcode & 0x000F) {
                    case 0x0000:
                        Arch->v[x] = Arch->v[y];
                        break;
                    case 0x0001:
                        Arch->v[x] |= Arch->v[y];
                        break;
                    case 0x0002:
                        Arch->v[x] &= Arch->v[y];
                        break;
                    case 0x0003:
                        Arch->v[x] ^= Arch->v[y];
                        break;
                    case 0x0004: {
                        uint16_t sum = Arch->v[x] + Arch->v[y];
                        uint8_t carry = (sum > 0xFF) ? 1 : 0;
                        Arch->v[x] = sum & 0xFF;
                        Arch->v[0xF] = carry;
                        break;
                    }
                    case 0x0005: {
                        uint8_t borrow = (Arch->v[x] >= Arch->v[y]) ? 1 : 0;
                        Arch->v[x] -= Arch->v[y];
                        Arch->v[0xF] = borrow;
                        break;
                    }
                    case 0x0006: {
                        uint8_t lsb = Arch->v[x] & 0x1;
                        Arch->v[x] >>= 1;
                        Arch->v[0xF] = lsb;
                        break;
                    }
                    case 0x0007: {
                        uint8_t borrow = (Arch->v[y] >= Arch->v[x]) ? 1 : 0;
                        Arch->v[x] = Arch->v[y] - Arch->v[x];
                        Arch->v[0xF] = borrow;
                        break;
                    }
                    case 0x000E: {
                        uint8_t msb = (Arch->v[x] & 0x80) >> 7;
                        Arch->v[x] <<= 1;
                        Arch->v[0xF] = msb;
                        break;
                    }
                }
                Arch->PC += 2;
                break;
            }
            case 0x9000: {
                if ((opcode & 0x000F) == 0x0000) {
                    if (Arch->v[(opcode & 0x0F00) >> 8] != Arch->v[(opcode & 0x00F0) >> 4]) {
                        Arch->PC += 4;
                    } else {
                        Arch->PC += 2;
                    }
                }
                break;
            }
            case 0xA000: {
                Arch->IR = (opcode & 0x0FFF);
                Arch->PC += 2;
                break;
            }
            case 0xB000: {
                Arch->PC = (opcode & 0x0FFF) + Arch->v[0x0];
                break;
            }
            case 0xC000: {
                uint8_t random_byte = rand() & 0xFF;
                Arch->v[(opcode & 0x0F00) >> 8] = random_byte & (opcode & 0x00FF);
                Arch->PC += 2;
                break;
            }
            case 0xD000: {
                uint8_t x = Arch->v[(opcode & 0x0F00) >> 8] % 64;
                uint8_t y = Arch->v[(opcode & 0x00F0) >> 4] % 32;
                uint8_t height = (opcode & 0x000F);

                Arch->v[0xF] = 0;

                for (int sprite_row = 0; sprite_row < height; sprite_row++) {
                    if (y + sprite_row >= 32) break; // Clip bottom

                    uint8_t pixel = Arch->main_memory[Arch->IR + sprite_row];

                    for (int sprite_col = 0; sprite_col < 8; sprite_col++) {
                        if (x + sprite_col >= 64) break; // Clip right

                        if ((pixel & (0x80 >> sprite_col)) != 0) {
                            int in_screen_pos = (x + sprite_col) + ((y + sprite_row) * 64);

                            if (Arch->frame_buffer[in_screen_pos] == 1) {
                                Arch->v[0xF] = 1;
                            }
                            Arch->frame_buffer[in_screen_pos] ^= 1;
                        }
                    }
                }
                draw_flag = 1;
                Arch->PC += 2;
                break;
            }
            case 0xE000: {
                uint8_t key_val = Arch->v[(opcode & 0x0F00) >> 8] & 0x0F;
                switch (opcode & 0x00FF) {
                    case 0x009E: {
                        Arch->PC += (keys[key_val] != 0) ? 4 : 2;
                        break;
                    }
                    case 0x00A1: {
                        Arch->PC += (keys[key_val] == 0) ? 4 : 2;
                        break;
                    }
                }
                break;
            }
            case 0xF000: {
                uint8_t x = (opcode & 0x0F00) >> 8;

                switch (opcode & 0x00FF) {
                    case 0x0007: {
                        Arch->v[x] = Arch->delay_timer;
                        Arch->PC += 2;
                        break;
                    }
                    case 0x000A: {
                        int key_pressed = 0;
                        for (int i = 0; i < 16; i++) {
                            if (keys[i] != 0) {
                                Arch->v[x] = i;
                                key_pressed = 1;
                                break;
                            }
                        }
                        if (key_pressed) {
                            Arch->PC += 2;
                        }
                        break;
                    }
                    case 0x0015: {
                        Arch->delay_timer = Arch->v[x];
                        Arch->PC += 2;
                        break;
                    }
                    case 0x0018: {
                        Arch->sound_timer = Arch->v[x];
                        Arch->PC += 2;
                        break;
                    }
                    case 0x001E: {
                        Arch->IR += Arch->v[x];
                        Arch->PC += 2;
                        break;
                    }
                    case 0x0029: {
                        Arch->IR = (Arch->v[x] & 0x0F) * 5;
                        Arch->PC += 2;
                        break;
                    }
                    case 0x0033: {
                        uint8_t val = Arch->v[x];
                        Arch->main_memory[Arch->IR]     = val / 100;
                        Arch->main_memory[Arch->IR + 1] = (val / 10) % 10;
                        Arch->main_memory[Arch->IR + 2] = val % 10;
                        Arch->PC += 2;
                        break;
                    }
                    case 0x0055: {
                        for (int i = 0; i <= x; i++) {
                            Arch->main_memory[Arch->IR + i] = Arch->v[i];
                        }
                        Arch->PC += 2;
                        break;
                    }
                    case 0x0065: {
                        for (int i = 0; i <= x; i++) {
                            Arch->v[i] = Arch->main_memory[Arch->IR + i];
                        }
                        Arch->PC += 2;
                        break;
                    }
                }
                break;
            }
        }
    } else {
        printf("\nProgram counter out of bounds!");
    }
}

void handle_timers(void) {
    if (Arch->delay_timer > 0) {
        Arch->delay_timer--;
    }
    if (Arch->sound_timer > 0) {
        if (Arch->sound_timer == 1) {
           //BEEP !
            printf("\nBEEP !");
        }
        Arch->sound_timer--;
    }
}
