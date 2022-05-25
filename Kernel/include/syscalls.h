#ifndef TPE_SYSCALLS_H
#define TPE_SYSCALLS_H

#include <video_driver.h>
#include <stdint.h>
#include <keyboard.h>
#include <scheduler.h>
uint8_t read_handler(char * str);                       // Lee por pantalla el siguiente caracter y lo copia en str
uint8_t write_handler(char* str, formatType format);    // Escribe el string str por pantalla con el formato format
uint8_t exec_handler(uint8_t cant, void** programas);                    // Agrega un nuevo proceso al arreglo
uint8_t exit_handler();                         // Finaliza un proceso con codigo de error

#endif
