
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <scheduler.h>

#define BUFF_LENGTH 100

void keyboard_handler();
uint8_t get_keyboard_scan_code();           // Funcion hecha en assembler para interactuar con el teclado
void restart_state();                       // Limpia el estado de los flags que verifican el estado de un proceso activo (suspendido o corriendo)



#endif
