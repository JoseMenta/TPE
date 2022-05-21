
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#define BUFF_LENGTH 100
void keyboard_handler();
uint8_t get_keyboard_scan_code(); //funcion hecha en assembler para interactuar con el teclado
//Estas al final no las implementamos aca
//int sys_read(char * buff);


#endif
