#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "defines.h"
#include "emu.c"

uint8_t fontset[80] = {
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

void chp8_clear_keypad(struct emu_t* emu) {
    for (int i = 0; i < 16; i++)
        emu->keypad[i] = false;
}

struct emu_t* chp8_init() {
    emu_t* emu  = calloc(sizeof(emu_t) + MEMSIZE * sizeof(uint8_t), 1);
    emu->pc     = PROGSTART;
    emu->memory = (uint8_t*) (emu + 1);

    // Copy the fontset to memory at offset 0
    for (int i = 0; i < 80; i++)
        emu->memory[i] = fontset[i];

    chp8_clear_keypad(emu);

    return emu;
}

void chp8_reset(struct emu_t* emu) {
    op_cls(emu);
    emu->pc     = PROGSTART;
    emu->V[0xF] = 0;
    emu->I      = 0;
    emu->SP     = 0;
    emu->DT     = 0;

    chp8_clear_keypad(emu);
}

void chp8_destroy(struct emu_t* emu) {
    free(emu);
}

bool chp8_load_from_file(struct emu_t* emu, const char* filename) {
    FILE *rom = fopen(filename, "r");
    if (!rom) {
        perror("Error opening the file");
        return false;
    }

    fseek(rom, 0L, SEEK_END);
    long file_size = ftell(rom);
    emu->current_rom_size = file_size;
    rewind(rom);

    printf("File size is: %ld\n", file_size);

    size_t bytes_read = fread(emu->memory + PROGSTART, sizeof(uint8_t), file_size, rom);

    if (bytes_read != file_size) {
        perror("Error reading from file");
        return false;
    }

    fclose(rom);
    return true;
}

void chp8_setkey(struct emu_t* emu, uint8_t key, bool status) {
    emu->keypad[key] = status;
}

bool chp8_singlestep(struct emu_t* emu) {
    if (emu->pc >= PROGSTART + emu->current_rom_size)
        return false;

    uint16_t instr = (uint16_t)(emu->memory[emu->pc] << 8 | emu->memory[emu->pc + 1]);
    emu->pc += 2;
    handle_instruction(emu, instr);

    // Update delay timer
    if (emu->DT > 0)
        emu->DT--;

    chp8_clear_keypad(emu);

    return true;
}
