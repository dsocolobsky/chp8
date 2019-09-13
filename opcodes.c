#include "defines.h"
#include <stdio.h>

void op_cls(emu_t *emu) {
    printf("op_cls()\n");
    for (int row = 0; row < DISPLAY_ROWS; row++)
    for (int col = 0; col < DISPLAY_COLS; col++) {
        emu->display[row][col] = 0;
    }
}

void op_ret(emu_t *emu) {
    printf("STUB op_ret()\n");
}

void op_jp(emu_t *emu, uint16_t addr) {
    printf("op_jp(addr: %04x)\n", addr);
    emu->pc = addr;
}

void op_call(emu_t *emu, uint16_t addr) {
    printf("STUB op_call(addr: %04x)\n", addr);
}

void op_se(emu_t *emu, uint8_t reg, uint16_t val) {
    printf("op_se(reg: %02x, val: %04x)\n", reg, val);
    if (emu->V[reg] == val) {
        emu->pc += 2;
    }
}

void op_sne(emu_t *emu, uint8_t reg, uint16_t val) {
    printf("op_sne(reg: %02x, val: %04x)\n", reg, val);
    if (emu->V[reg] != val) {
        emu->pc += 2;
    }
}

void op_se_regs(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("op_se(reg1: %02x, reg2: %02x)\n", reg1, reg2);
    if (emu->V[reg1] == emu->V[reg2]) {
        emu->pc += 2;
    }
}

void op_ld(emu_t *emu, uint8_t reg, uint16_t val) {
    printf("op_ld(reg: %02X, val: %04X)\n", reg, val);
    printf("old V%1X was %02X now is ", reg, emu->V[reg]);
    emu->V[reg] = val;
    printf("%02X\n", emu->V[reg]);
}

void op_add(emu_t *emu, uint8_t reg, uint16_t val) {
    printf("op_add(reg: %02X, val: %02X)\n", reg, val);
    printf("old V%1X was %02X now is ", reg, emu->V[reg]);
    emu->V[reg] += val;
    printf("%02X\n", emu->V[reg]);
}

void op_ld_regs(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("op_ld(reg1: %02x, reg2: %02x)\n", reg1, reg2);
    emu->V[reg1] = emu->V[reg2];
}

void op_or(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("op_or(reg1: %02x, reg2: %02x)\n", reg1, reg2);
    emu->V[reg1] |= emu->V[reg2];
}

void op_and(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("op_and(reg1: %02x, reg2: %02x)\n", reg1, reg2);
    emu->V[reg1] &= emu->V[reg2];
}

void op_xor(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("op_xor(reg1: %02x, reg2: %02x)\n", reg1, reg2);
    emu->V[reg1] ^= emu->V[reg2];
}

void op_add_regs(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("op_add_regs(reg1: %02x, reg2: %02x)\n", reg1, reg2);
    uint16_t res = emu->V[reg1] + emu->V[reg2];
    if (res > 255) {
        emu->VF = 1;
        emu->V[reg1] = (uint8_t)(res);
    } else {
        emu->VF = 0;
        emu->V[reg1] = res;
    }
}

void op_sub(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("op_sub(reg1: %02x, reg2: %02x)\n", reg1, reg2);
    emu->VF = emu->V[reg1] > emu->V[reg2] ? 1 : 0;
    emu->V[reg1] -= emu->V[reg2];
}

// WARNING according to cowgod's this is the implementation of shr
// But some people say it's rather Vx = Vy >> 1
void op_shr(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("STUB op_shr(reg1: %02x, reg2: %02x)\n", reg1, reg2);
    emu->V[reg1] >>= 1;
}

void op_subn(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("STUB op_subn(reg1: %02x, reg2: %02x)\n", reg1, reg2);
}

void op_shl(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("STUB op_shl(reg1: %02x, reg2: %02x)\n", reg1, reg2);
}

void op_sne_regs(emu_t *emu, uint8_t reg1, uint8_t reg2) {
    printf("STUB op_sne_regs(reg1: %02x, reg2: %02x)\n", reg1, reg2);
}

void op_ld_I(emu_t *emu, uint16_t addr) {
    printf("op_ld_I(addr: %04X)\n", addr);
    emu->I = addr;
}

void op_jp_V0(emu_t *emu, uint16_t addr) {
    printf("STUB op_jp_V0(addr: %04X)\n", addr);
    emu->I = addr;
}

void op_rnd(emu_t *emu, uint8_t reg, uint16_t val) {
    printf("STUB op_rnd(reg: %02X, val: %04X)\n", reg, val);
}

void op_display(emu_t *emu, uint8_t x, uint8_t y, uint8_t n) {
    printf("op_display(x: %02X, y: %02X, lines: %02X)\n", x, y, n);

    emu->VF = 0;
    /* Each line of a sprite is 8 pixels => it's 1 byte */
    for (int line = 0; line < n; line++) {
        for (int px = 7; px >= 0; px--) {
            if (BIT_SET(emu->memory[emu->I+line], px)) {
                uint8_t rx = (x+px)%64;
                uint8_t ry = (y+line)%32;
                if (emu->display[ry][rx] == 1) {
                    emu->VF = 1;
                }
                emu->display[ry][rx] ^= 1;
            }
        }
    }
}

void op_skp(emu_t *emu, uint8_t reg) {
    printf("STUB op_skp(reg: %02X)\n", reg);
}

void op_skpn(emu_t *emu, uint8_t reg) {
    printf("STUB op_skpn(reg: %02X)\n", reg);
}

void op_ld_Vx_DT(emu_t *emu, uint8_t reg) {
    printf("STUB op_ld_Vx_DT(reg: %02X)\n", reg);
}

void op_ld_Vx_K(emu_t *emu, uint8_t reg) {
    printf("STUB op_ld_Vx_K(reg: %02X)\n", reg);
}

void op_ld_DT_Vx(emu_t *emu, uint8_t reg) {
    printf("STUB op_ld_DT_Vx(reg: %02X)\n", reg);
}

void op_ld_ST_Vx(emu_t *emu, uint8_t reg) {
    printf("STUB op_ld_ST_Vx(reg: %02X)\n", reg);
}

void op_add_I(emu_t *emu, uint8_t reg) {
    printf("STUB op_add_I(reg: %02X)\n", reg);
}

void op_ld_F(emu_t *emu, uint8_t reg) {
    printf("STUB op_ld_F(reg: %02X)\n", reg);
}

void op_ld_BCD(emu_t *emu, uint8_t reg) {
    printf("STUB op_ld_BCD(reg: %02X)\n", reg);
}

void op_ld_array_Vx(emu_t *emu, uint8_t reg) {
    printf("STUB op_ld_array_Vx(reg: %02X)\n", reg);
}

void op_ld_Vx_array(emu_t *emu, uint8_t reg) {
    printf("STUB op_ld_Vx_array(reg: %02X)\n", reg);
}



