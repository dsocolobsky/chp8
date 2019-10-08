#ifndef _CHP8_H
#define _CHP8_H
#include <stdbool.h>

/* chp8_init:
 *   Inicializa las estructuras requeridas para correr el emulador. Éstas
 *   pueden devolverse con `chp8_destroy(struct emu_t*)`.
 */
struct emu_t* chp8_init();

/* chp8_reset:
 *   Reinicia el emulador modificando el contador de programa y los registros
 *   T y VF.
 */
void chp8_reset(struct emu_t* emu);

void chp8_clear_keypad(struct emu_t* emu);

/* chp8_destroy:
 *   Devuelve los recursos adquiridos por chp8_init().
 */
void chp8_destroy(struct emu_t* emu);

/* chp8_load_from_file:
 *   Carga a la memoria del emulador los contenidos del archivo `filename`.
 *   Devuelve false en caso de error.
 */
bool chp8_load_from_file(struct emu_t* emu, const char* filename);

/* chp8_singlestep:
 *   Emula una única instrucción si es posible, devuelve false si la ejecución
 *   terminó.
 */
bool chp8_singlestep(struct emu_t* emu);
#endif
