#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../raylib/raylib/src/raylib.h"
#include "defs.h"

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define SCALE 15

extern architecture *Arch;
extern uint8_t keys[16];

// Standard CHIP-8 to QWERTY mapping
void handle_input(void) {
    keys[0x1] = IsKeyDown(KEY_ONE);   keys[0x2] = IsKeyDown(KEY_TWO);   keys[0x3] = IsKeyDown(KEY_THREE); keys[0xC] = IsKeyDown(KEY_FOUR);
    keys[0x4] = IsKeyDown(KEY_Q);     keys[0x5] = IsKeyDown(KEY_W);     keys[0x6] = IsKeyDown(KEY_E);     keys[0xD] = IsKeyDown(KEY_R);
    keys[0x7] = IsKeyDown(KEY_A);     keys[0x8] = IsKeyDown(KEY_S);     keys[0x9] = IsKeyDown(KEY_D);     keys[0xE] = IsKeyDown(KEY_F);
    keys[0xA] = IsKeyDown(KEY_Z);     keys[0x0] = IsKeyDown(KEY_X);     keys[0xB] = IsKeyDown(KEY_C);     keys[0xF] = IsKeyDown(KEY_V);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <path_to_rom.ch8>\n", argv[0]);
        return -1;
    }

    initialize_chip8(argv[1]);

    const int windowWidth = CHIP8_WIDTH * SCALE;
    const int windowHeight = CHIP8_HEIGHT * SCALE;
    
    InitWindow(windowWidth, windowHeight, "CHIP-8 Emulator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        handle_input();

        // Run 9 cycles per frame (~540Hz CPU speed)
        for (int i = 0; i < 9; i++) {
            emulate_cycle();
        }

        // Decrement 60Hz delay and sound timers once per frame
        handle_timers();

        BeginDrawing();
            ClearBackground(BLACK);

            if (Arch != NULL) {
                for (int y = 0; y < CHIP8_HEIGHT; y++) {
                    for (int x = 0; x < CHIP8_WIDTH; x++) {
                        if (Arch->frame_buffer[x + (y * CHIP8_WIDTH)] != 0) {
                            DrawRectangle(
                                x * SCALE,
                                y * SCALE,
                                SCALE,
                                SCALE,
                                LIME
                            );
                        }
                    }
                }
            }
        EndDrawing();
    }

    CloseWindow();

    if (Arch != NULL) {
        free(Arch);
        Arch = NULL;
    }

    printf("\nEmulator shutdown cleanly. Goodbye!\n");
    return 0;
}