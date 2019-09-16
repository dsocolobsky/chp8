#include <stdarg.h>

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

static inline
const char* current_instr(emu_t *emu) {
    uint16_t ins = (uint16_t)(emu->memory[emu->pc] << 8 | emu->memory[emu->pc + 1]);
    uint8_t val  = BYTE_0(ins);

    switch (NIBBLE_3(ins)) {
        case 0x0:
            if (val == 0xE0)
                return "op_cls(emu)";
            else if (val == 0xEE)
                return "op_ret(emu)";
        case 0x1: return "op_jp(emu, addr)";
        case 0x2: return "op_call(emu, addr)";
        case 0x3: return "op_se(emu, reg1, val)";
        case 0x4: return "op_sne(emu, reg1, val)";
        case 0x5: return "op_se_regs(emu, reg1, reg2)";
        case 0x6: return "op_ld(emu, reg1, val)";
        case 0x7: return "op_add(emu, reg1, val)";
        case 0x8:
            switch (NIBBLE_0(ins)) {
                case 0x0: return "op_ld_regs(emu, reg1, reg2)";
                case 0x1: return "op_or(emu, reg1, reg2)";
                case 0x2: return "op_and(emu, reg1, reg2)";
                case 0x3: return "op_xor(emu, reg1, reg2)";
                case 0x4: return "op_add_regs(emu, reg1, reg2)";
                case 0x5: return "op_sub(emu, reg1, reg2)";
                case 0x6: return "op_shr(emu, reg1, reg2)";
                case 0x7: return "op_subn(emu, reg1, reg2)";
                case 0xE: return "op_shl(emu, reg1, reg2)";
            }
            break;
        case 0x9: return "op_sne(emu, reg1, reg2)";
        case 0xA: return "op_ld_I(emu, addr)";
        case 0xB: return "op_jp_V0(emu, addr)";
        case 0xC: return "op_rnd(emu, reg1, val)";
        case 0xD: return "op_display(emu, reg1, reg2, NIBBLE_0(ins))";
        case 0xE: return (val == 0x9E) ? "op_skp(emu, reg1)" : "op_skpn(emu, reg2)";
        case 0xF:
            switch (val) {
                case 0x07: return "op_ld_Vx_DT(emu, reg1)";
                case 0x0A: return "op_ld_Vx_K(emu, reg1)";
                case 0x15: return "op_ld_DT_Vx(emu, reg1)";
                case 0x18: return "op_ld_ST_Vx(emu, reg1)";
                case 0x1E: return "op_add_I(emu, reg1)";
                case 0x29: return "op_ld_F(emu, reg1)";
                case 0x33: return "op_ld_BCD(emu, reg1)";
                case 0x55: return "op_ld_array_Vx(emu, reg1)";
                case 0x65: return "op_ld_Vx_array(emu, reg1)";
            }
    }
    return "??? ERROR ???";
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

void chp8_display_window(struct nk_context *ctx, struct emu_t* emu) {
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
