#ifndef TPE_BASH_H
#define TPE_BASH_H


#include <libc.h>


#define LOGOUT ("logout")             // El string con el cual se debe finalizar bash
#define ASCII_DELETE (127)            // El valor ASCII de la tecla borrar
#define MAX_BUFFER_SIZE (100)         // Tamaño maximo de caracteres que puede almacenar el buffer
#define MAX_ARGS_SIZE (2)             // La cantidad máxima de argumentos que vamos a aceptar en un programa
//#define NOTHING_CYCLES (2000000)      // Ciclos que deben trasncurrir para realizar un parpadeo
//#define NOTHING_CYCLES (1000000)
void bash(uint64_t arg_c, const char ** arg_v);

#endif