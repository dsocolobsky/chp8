#ifndef _CHP8_H
#define _CHP8_H
#include <stdbool.h>

/* Initializes the emulator which then can be freed by using chp8_destroy */
struct emu_t* chp8_init();

/* Restarts the emulator to the initial state zeroing the required registers */
void chp8_reset(struct emu_t* emu);

/* Restarts the keypad to the initial state (all keys not pressed) */
void chp8_clear_keypad(struct emu_t* emu);

/* Frees the allocated memory needed by the emulator */
void chp8_destroy(struct emu_t* emu);

/* Loads the emulator with the ROM specified in the path filename, returns false on error */
bool chp8_load_from_file(struct emu_t* emu, const char* filename);

/* Sets the status of a key in the keypad to 'status' which represents if it's pressed or not */
void chp8_setkey(struct emu_t* emu, uint8_t key, bool status);

/* Emulates a single instruction (if possible) and returns False if the execution ended */
bool chp8_singlestep(struct emu_t* emu);

#endif
