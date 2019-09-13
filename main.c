#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "defines.h"

emu_t emu;

void show_display() {
    printf("BEGIN DISPLAY\n");
    for (int row = 0; row < DISPLAY_ROWS; row++, printf("\n"))
    for (int col = 0; col < DISPLAY_COLS; col++) {
        if (emu.display[row][col] == 1) {
            printf("X");
        } else {
            printf("_");
        }
    }
    printf("END DISPLAY\n");
}


void handle_instruction(uint16_t ins) {
    uint8_t reg1 = NIBBLE_2(ins);
    uint8_t reg2 = NIBBLE_1(ins);
    uint8_t val  = BYTE_0(ins);
    uint8_t addr = ins & 0x0FFF;

    switch (NIBBLE_3(ins)) {
        case 0x0: if (val == 0xE0) op_cls(&emu); else if (val == 0xEE) op_ret(&emu); break;
        case 0x1: op_jp      (&emu, addr);       break;
        case 0x2: op_call    (&emu, addr);       break;
        case 0x3: op_se      (&emu, reg1, val);  break;
        case 0x4: op_sne     (&emu, reg1, val);  break;
        case 0x5: op_se_regs (&emu, reg1, reg2); break;
        case 0x6: op_ld      (&emu, reg1, val);  break;
        case 0x7: op_add     (&emu, reg1, val);  break;
        case 0x8:
            switch (NIBBLE_0(ins)) {
                case 0x1: op_ld_regs  (&emu, reg1, reg2); break;
                case 0x2: op_and      (&emu, reg1, reg2); break;
                case 0x3: op_xor      (&emu, reg1, reg2); break;
                case 0x4: op_add_regs (&emu, reg1, reg2); break;
                case 0x5: op_sub      (&emu, reg1, reg2); break;
                case 0x6: op_shr      (&emu, reg1, reg2); break;
                case 0x7: op_subn     (&emu, reg1, reg2); break;
                case 0xE: op_shl      (&emu, reg1, reg2); break;
            }
            break;
        case 0x9: op_sne    (&emu, reg1, reg2); break;
        case 0xA: op_ld_I   (&emu, addr);       break;
        case 0xB: op_jp_V0  (&emu, addr);       break;
        case 0xC: op_rnd    (&emu, reg1, val);  break;
        case 0xD: op_display(&emu, reg1, reg2, NIBBLE_0(ins)); show_display();   break;
        case 0xE: if (val == 0x9E) op_skp(&emu, reg1); else op_skpn(&emu, reg2); break;
        case 0xF:
            switch (val) {
                case 0x07: op_ld_Vx_DT   (&emu, reg1); break;
                case 0x0A: op_ld_Vx_K    (&emu, reg1); break;
                case 0x15: op_ld_DT_Vx   (&emu, reg1); break;
                case 0x18: op_ld_ST_Vx   (&emu, reg1); break;
                case 0x1E: op_add_I      (&emu, reg1); break;
                case 0x29: op_ld_F       (&emu, reg1); break;
                case 0x33: op_ld_BCD     (&emu, reg1); break;
                case 0x55: op_ld_array_Vx(&emu, reg1); break;
                case 0x65: op_ld_Vx_array(&emu, reg1); break;
            }
            break;
        default: printf("INSTRUCTION %04X NOT FOUND\n", ins); break;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Debes pasar el nombre de la ROM\n");
        return 1;
    }

    FILE *rom;
    rom = fopen(argv[1], "r");
    if (!rom) {
        printf("Error al leer archivo\n");
        return 1;
    }

    emu.pc = PROGSTART;
    emu.VF = 0;
    emu.I  = PROGSTART;
    emu.memory = calloc(MEMSIZE, sizeof(uint8_t));

    fseek(rom, 0L, SEEK_END);
    long file_size = ftell(rom);
    rewind(rom);

    printf("El size del fichero es: %ld\n", file_size);

    size_t bytes_read = fread(emu.memory+PROGSTART, sizeof(uint8_t), file_size, rom);
    if (bytes_read != file_size) {
        printf("Error leyendo bytes del archivo\n");
        return 1;
    }

    uint16_t instruction;
    while (emu.pc < PROGSTART+file_size) {
        instruction = (uint16_t)(emu.memory[emu.pc]<<8 | emu.memory[emu.pc+1]);
        emu.pc+=2;
        handle_instruction(instruction);
    }

    show_display();

    return 0;
}