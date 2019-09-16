#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "defines.h"
#include "emu.c"

struct emu_t* chp8_init() {
    emu_t* emu = calloc(sizeof(emu_t) + MEMSIZE * sizeof(uint8_t), 1);
    emu->pc = PROGSTART;
    emu->memory = (uint8_t*) (emu + 1);

    return emu;
}

void chp8_reset(struct emu_t* emu) {
    emu->pc = PROGSTART;
    emu->VF = 0;
    emu->I = 0;
}

void chp8_destroy(struct emu_t* emu) {
    free(emu);
}

bool chp8_load_from_file(struct emu_t* emu, const char* filename) {
    FILE *rom;
    rom = fopen(filename, "r");

    if (!rom) {
        perror("Error al leer archivo");
        return false;
    }

    fseek(rom, 0L, SEEK_END);
    long file_size = ftell(rom);
    emu->current_rom_size = file_size;
    rewind(rom);

    printf("El size del fichero es: %ld\n", file_size);

    size_t bytes_read = fread(emu->memory + PROGSTART, sizeof(uint8_t), file_size, rom);

    if (bytes_read != file_size) {
        perror("Error leyendo bytes del archivo");
        return false;
    }


    fclose(rom);

    return true;
}

bool chp8_singlestep(struct emu_t* emu) {
    if (emu->pc >= PROGSTART + emu->current_rom_size)
        return false;

    uint16_t instr = (uint16_t)(emu->memory[emu->pc] << 8 | emu->memory[emu->pc + 1]);
    emu->pc += 2;
    printf("pc: %04X\n", emu->pc);
    handle_instruction(emu, instr);

    return true;
}
