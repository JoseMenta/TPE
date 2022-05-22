#ifndef TPE_SYSCALLS_H
#define TPE_SYSCALLS_H

#include <video_driver.h>
#include <stdint.h>
#include <keyboard.h>

int read_handler(char * str);                       // Lee por pantalla el siguiente caracter y lo copia en str
int write_handler(char* str, formatType format);    // Escribe el string str por pantalla con el formato format
int exec_handler(void* program);                    // Agrega un nuevo proceso al arreglo
int exit_handler(int code);                         // Finaliza un proceso con codigo de error

#endif
