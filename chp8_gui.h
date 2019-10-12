#ifndef _CHP8_GUI_H
#define _CHP8_GUI_H
/** Action to perform in the emulator:
 *   - CONTINUE: Set state to RUNNING
 *   - STEP:     Single-step
 *   - PAUSE:    Set state to PAUSED
 *   - NONE:     Do nothing
**/
enum action { CONTINUE, STEP, PAUSE, RESET, NONE };

/* State of the emulator:
 *   - RUNNING: Emulator is currently running and can be paused
 *   - PAUSED: Emulator is currently running and can be un-paused (or single-stepped)
 */
enum status { RUNNING, PAUSED };

/* Shows the register window and flow of the emulator. Returns the action to perform */
enum action chp8_debug_window(struct nk_context *ctx, struct emu_t* emu,
                              enum status status);

/* Shows the emulator's display (as text)  CURRENTLY DISABLED */
void chp8_display_window(struct nk_context *ctx, struct emu_t* emu);

/* Show the ROM's disassembly and the current instruction */
void chp8_code_window(struct nk_context* ctx, struct emu_t* emu);

/* Shows the emulator's memory as raw bytes */
void chp8_memory_window(struct nk_context* ctx, struct emu_t* emu);

/* Shows the CHIP8 keypad, which can be interacted with */
void chp8_keypad_window(struct nk_context* ctx, struct emu_t* emu);

#endif
