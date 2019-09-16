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
    uint8_t V[15];
    uint8_t VF;
    uint16_t DT;
    uint16_t ST;
    uint8_t SP;
    uint16_t stack[48];
    uint8_t display[DISPLAY_ROWS][DISPLAY_COLS];
    uint32_t current_rom_size;
} emu_t;
#endif
