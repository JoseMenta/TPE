#ifndef TPE_SYSCALLS_H
#define TPE_SYSCALLS_H

#include <video_driver.h>
#include <stdint.h>
#include <keyboard.h>
#include <scheduler.h>

typedef enum {SEC = 0, MIN = 2, HOUR = 4, DAY_WEEK = 6, DAY_MONTH = 7, MONTH = 8, YEAR = 9} timeType;

uint8_t read_handler(char * str);                       // Lee por pantalla el siguiente caracter y lo copia en str
uint8_t write_handler(const char* str, formatType format);    // Escribe el string str por pantalla con el formato format
uint8_t exec_handler(uint8_t cant, void** programas);                    // Agrega un nuevo proceso al arreglo
uint8_t exit_handler();                                 // Finaliza un proceso con codigo de error
uint8_t time_handler(timeType time_unit);               // Devuelve el valor para la unidad indicada en GMT

uint8_t get_time(timeType time_unit);                    // Llama al RTC con la unidad indicada

#endif
