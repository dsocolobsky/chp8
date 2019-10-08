#ifndef _CHP8_GUI_H
#define _CHP8_GUI_H
/* Acción a realizar sobre el emulador:
 *   - CONTINUE: Pasar el estado a RUNNING
 *   - STEP: Realizar single-stepping
 *   - PAUSE: Pasar el estado a PAUSED
 *   - NONE: No hacer nada */
enum action { CONTINUE, STEP, PAUSE, RESET, NONE };

/* Estado del eumlador:
 *   - RUNNING: Indica que el emulador esta corriendo actualmente y puede ser
 *              pausado.
 *   - PAUSED: Indica que el emulador está pausado actualmente y puede
 *             despausarse o realizar single-stepping
 */
enum status { RUNNING, PAUSED };

/* chp8_debug_window:
 *   Muestra la ventana de registros y control de flujo del emulador. Devuelve
 *   la acción a ejecutar.
 */
enum action chp8_debug_window(struct nk_context *ctx, struct emu_t* emu,
                              enum status status);

/* chp8_display_window:
 *   Muestra el display del emulador (texto).
 */
void chp8_display_window(struct nk_context *ctx, struct emu_t* emu);

/* chp8_code_window:
 *   Muestra una versión desensamblada de las próximas instrucciones
 */
void chp8_code_window(struct nk_context* ctx, struct emu_t* emu);

void chp8_memory_window(struct nk_context* ctx, struct emu_t* emu);

void chp8_keypad_window(struct nk_context* ctx, struct emu_t* emu);

#endif
