#include <stdarg.h>
#include <stdio.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear/nuklear.h"

#include "defines.h"
#include "chp8_gui.h"

static
const char* instr_to_string(uint16_t ins) {
    static char result[64];
    #define INS(...) ((void)(snprintf(result, 64, __VA_ARGS__)), result)
    uint8_t reg1 = NIBBLE_2(ins);
    uint8_t reg2 = NIBBLE_1(ins);
    uint8_t val  = BYTE_0(ins);
    uint16_t addr = ins & 0x0FFF;

    switch (NIBBLE_3(ins)) {
        case 0x0:
            if (val == 0xE0)
                return INS("CLS");
            else if (val == 0xEE)
                return (snprintf(result, 64, "RES"), result);
        case 0x1: return INS("JP 0x%04X", addr);
        case 0x2: return INS("CALL 0x%04X", addr);
        case 0x3: return INS("SE V%X, %03i", reg1, val);
        case 0x4: return INS("SNE V%X, %04i", reg1, val);
        case 0x5: return INS("SE V%X, V%X", reg1, reg2);
        case 0x6: return INS("LD V%X, %03i", reg1, val);
        case 0x7: return INS("ADD V%X, %03i", reg1, val);
        case 0x8:
            switch (NIBBLE_0(ins)) {
                case 0x0: return INS("LD V%X, V%X", reg1, reg2);
                case 0x1: return INS("OR V%X. V%X", reg1, reg2);
                case 0x2: return INS("AND V%X, V%X", reg1, reg2);
                case 0x3: return INS("XOR V%X, V%X", reg1, reg2);
                case 0x4: return INS("ADD V%X, V%X", reg1, reg2);
                case 0x5: return INS("SUB V%X. V%X", reg1, reg2);
                case 0x6: return INS("SHR V%X, V%X", reg1, reg2);
                case 0x7: return INS("SUBN V%X, V%X", reg1, reg2);
                case 0xE: return INS("SHL V%X, V%X", reg1, reg2);
            }
            break;
        case 0x9: return INS("SNE V%X, V%X", reg1, reg2);
        case 0xA: return INS("LD I, 0x%04X", addr);
        case 0xB: return INS("JP V0, 0x%04X", addr);
        case 0xC: return INS("RND V%X, %03i", reg1, val);
        case 0xD: return INS("DRW V%X, V%X, %03i", reg1, reg2, NIBBLE_0(ins));
        case 0xE: return (val == 0x9E)
	                 ? INS("SKP V%X", reg1)
			 : INS("SKNP V%X", reg1);
        case 0xF:
            switch (val) {
                case 0x07: return INS("LD V%X, DT", reg1);
                case 0x0A: return INS("LD V%X, K", reg1);
                case 0x15: return INS("LD DT, V%X", reg1);
                case 0x18: return INS("LD ST, V%X", reg1);
                case 0x1E: return INS("ADD I, V%X", reg1);
                case 0x29: return INS("LD F, V%X", reg1);
                case 0x33: return INS("LD B, V%X", reg1);
                case 0x55: return INS("LD [I], V%X", reg1);
                case 0x65: return INS("LD V%X, [I]", reg1);
            }
    }
    return INS("???");
    #undef INS
}

static inline
const char* current_instr(emu_t *emu) {
    uint16_t ins =
        (uint16_t)(emu->memory[emu->pc] << 8 | emu->memory[emu->pc + 1]);
    return instr_to_string(ins);
}

#define EMU_PROP(reg) \
    nk_labelf(ctx, NK_TEXT_ALIGN_LEFT, #reg ": %X", emu->reg)

enum action chp8_debug_window(struct nk_context *ctx, struct emu_t* emu,
                              enum status status) {
    enum action action = NONE;

    if (nk_begin(ctx, "CHP8 Debug", nk_rect(50, 50, 300, 460),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 0, 2);
        if (status == PAUSED && nk_button_label(ctx, "cont"))
            action = CONTINUE;
        if (status == PAUSED && nk_button_label(ctx, "step"))
            action = STEP;
        if (status == RUNNING && nk_button_label(ctx, "pause"))
            action = PAUSE;
        nk_layout_row_dynamic(ctx, 0, 2);
	if (nk_button_label(ctx, "reset"))
	    action = RESET;

        nk_layout_row_dynamic(ctx, 0, 2);
        EMU_PROP(pc);
        EMU_PROP(I);
	nk_layout_row_dynamic(ctx, 0, 3);
        EMU_PROP(V[0x0]);
        EMU_PROP(V[0x1]);
        EMU_PROP(V[0x2]);
        EMU_PROP(V[0x3]);
        EMU_PROP(V[0x4]);
        EMU_PROP(V[0x5]);
        EMU_PROP(V[0x6]);
        EMU_PROP(V[0x7]);
        EMU_PROP(V[0x8]);
        EMU_PROP(V[0x9]);
        EMU_PROP(V[0xA]);
        EMU_PROP(V[0xB]);
        EMU_PROP(V[0xC]);
        EMU_PROP(V[0xD]);
        EMU_PROP(V[0xE]);
	nk_layout_row_dynamic(ctx, 0, 3);
        EMU_PROP(V[0xF]);
        EMU_PROP(DT);
        EMU_PROP(ST);
	nk_layout_row_dynamic(ctx, 0, 1);
	nk_labelf(ctx, NK_TEXT_ALIGN_LEFT, "Curr ins: %04X - %s",
                  (uint16_t)(emu->memory[emu->pc] << 8 | emu->memory[emu->pc + 1]),
                  current_instr(emu));
    }
    nk_end(ctx);

    return action;
}

void chp8_display_window(struct nk_context* ctx, struct emu_t* emu) {
    static char line[DISPLAY_COLS + 1];

    if (nk_begin(ctx, "CHP8 Display", nk_rect(450, 70, 600, 600),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 10, 1);
	for (int row = 0; row < DISPLAY_ROWS; row++) {
		for (int col = 0; col < DISPLAY_COLS; col++)
			line[col] = emu->display[row][col] ? 'X' : '-';
		nk_label(ctx, line, NK_TEXT_ALIGN_LEFT);
	}
    }
    nk_end(ctx);
}

void chp8_code_window(struct nk_context* ctx, struct emu_t* emu) {
    #define CODE_LINES 32

    if (nk_begin(ctx, "CHP8 Disassembly", nk_rect(700, 70, 300, 600),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 10, 1);
	for (int i = 0; i < 2 * CODE_LINES; i += 2) {
            uint16_t ins = (uint16_t)(emu->memory[emu->pc + i] << 8
                                      | emu->memory[emu->pc + i + 1]);
	    nk_labelf(ctx, NK_TEXT_ALIGN_LEFT, "0x%04X | %04X | %s",
	              emu->pc + i, ins, instr_to_string(ins));
	}
    }
    nk_end(ctx);
}
