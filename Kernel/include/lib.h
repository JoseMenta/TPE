#ifndef LIB_H
#define LIB_H

#include <stdint.h>

// Dada una direccion de memoria d, un entero z y una longitud n, setea el valor z desde [d] hasta [d+n-1] 
void * memset(void * destination, int32_t character, uint64_t length);

// Dada una direccion de memoria d, otra direccion s y una longitud n, copia el valor de [s] hasta [s+n-1] en [d] hasta [d+n-1]
void * memcpy(void * destination, const void * source, uint64_t length);

// Devuelve un string del nombre del fabricante del procesador (por ejemplo, si es Intel devuelve el string "Genuine Intel")
char *cpuVendor(char *result);

#endif