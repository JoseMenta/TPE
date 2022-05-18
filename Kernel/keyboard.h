
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
void keyboard_handler();
int sys_read(char * buff);
uint8_t get_keyboard_scan_code();

#endif
