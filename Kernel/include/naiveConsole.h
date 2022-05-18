#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

void ncPrint(const char * string);                      // Imprime un string pasado por parametros
void ncPrintChar(char character);                       // Imprime un char pasado por parametros
void ncNewline();                                       // Realiza un salto de linea
void ncPrintDec(uint64_t value);                        // Imprime un numero en formato decimal
void ncPrintHex(uint64_t value);                        // Imprime un numero en formato hexadecimal
void ncPrintBin(uint64_t value);                        // Imprime un numero en formato binario
void ncPrintBase(uint64_t value, uint32_t base);        // Imprime un numero en base pasado por parametros
void ncClear();                                         // Borra todo lo que aparezca en pantalla

#endif