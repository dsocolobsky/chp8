#ifndef _DEFINES_H
#define _DEFINES_H
#include <stdint.h>
#include <stdbool.h>

/* Memory size (in bytes) */
#define MEMSIZE     4096
/* Programs start on 512B onwards */
#define PROGSTART   512

/* Cols and Rows for the Chip8 display */
#define DISPLAY_COLS    64
#define DISPLAY_ROWS    32

// Actual screen in canvas values
#define CANVAS_PX_SIZE  8
#define CANVAS_WIDTH    DISPLAY_COLS*CANVAS_PX_SIZE
#define CANVAS_HEIGHT   DISPLAY_ROWS*CANVAS_PX_SIZE
#define CANVAS_X        100
#define CANVAS_Y        100
#define CANVAS_REFRESH  0

#define NIBBLE_3(X) (X & 0xF000)>>12
#define NIBBLE_2(X) (X & 0x0F00)>>8
#define NIBBLE_1(X) (X & 0x00F0)>>4
#define NIBBLE_0(X) (X & 0x000F)>>0

#define BYTE_1(X)   (X & 0xFF00)>>8
#define BYTE_0(X)   (X & 0x00FF)>>0

#define BIT_SET(X,N)    (X & (1<<N))

typedef struct emu_t {
    uint8_t  *memory;
    uint16_t pc;
    uint16_t I;
    uint8_t  V[16];
    uint16_t DT;
    uint16_t ST;
    uint8_t  SP;
    uint16_t stack[48];
    uint8_t  display[DISPLAY_ROWS][DISPLAY_COLS];
    uint32_t current_rom_size;
    bool keypad[16];
} emu_t;

#endif
