#ifndef _DEFINES_H
#define _DEFINES_H

#include <stdint.h>

/* Size de la memoria (en bytes) */
#define MEMSIZE     4096
/* Los programas comienzan a partir de los 512B */
#define PROGSTART   512

#define DISPLAY_COLS    64
#define DISPLAY_ROWS    32

#define NIBBLE_3(X) (X & 0xF000)>>12
#define NIBBLE_2(X) (X & 0x0F00)>>8
#define NIBBLE_1(X) (X & 0x00F0)>>4
#define NIBBLE_0(X) (X & 0x000F)>>0

#define BYTE_1(X)   (X & 0xFF00)>>8
#define BYTE_0(X)   (X & 0x00FF)>>0

#define BIT_SET(X,N)    (X & (1<<N))

typedef struct emu_t {
    uint8_t *memory;
    uint16_t pc;
    uint16_t I;
    uint16_t V[16];
    uint16_t VF;
    uint8_t display[DISPLAY_ROWS][DISPLAY_COLS];
} emu_t;

void op_cls     (emu_t *emu);
void op_ret     (emu_t *emu);
void op_jp      (emu_t *emu, uint16_t addr);
void op_call    (emu_t *emu, uint16_t addr);
void op_se      (emu_t *emu, uint8_t reg,   uint16_t val);
void op_sne     (emu_t *emu, uint8_t reg,   uint16_t val);
void op_se_regs (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_ld      (emu_t *emu, uint8_t reg,   uint16_t val);
void op_add     (emu_t *emu, uint8_t reg,   uint16_t val);
void op_ld_regs (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_or      (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_and     (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_xor     (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_add_regs(emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_sub     (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_shr     (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_subn    (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_shl     (emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_sne_regs(emu_t *emu, uint8_t reg1,  uint8_t reg2);
void op_ld_I    (emu_t *emu, uint16_t addr);
void op_jp_V0   (emu_t *emu, uint16_t addr);
void op_rnd     (emu_t *emu, uint8_t reg,   uint16_t val);
void op_display (emu_t *emu, uint8_t x,     uint8_t y, uint8_t n);
void op_skp     (emu_t *emu, uint8_t reg);
void op_skpn    (emu_t *emu, uint8_t reg);
void op_ld_Vx_DT(emu_t *emu, uint8_t reg);
void op_ld_Vx_K (emu_t *emu, uint8_t reg);
void op_ld_DT_Vx(emu_t *emu, uint8_t reg);
void op_ld_ST_Vx(emu_t *emu, uint8_t reg);
void op_add_I   (emu_t *emu, uint8_t reg);
void op_ld_F    (emu_t *emu, uint8_t reg);
void op_ld_BCD  (emu_t *emu, uint8_t reg);
void op_ld_Vx_array(emu_t *emu, uint8_t reg);
void op_ld_array_Vx(emu_t *emu, uint8_t reg);

#endif