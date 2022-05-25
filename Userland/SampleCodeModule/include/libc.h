
#ifndef TPE_LIBC_H
#define TPE_LIBC_H

#include <stdint.h>

typedef enum {BLACK=0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, PINK, YELLOW, WHITE} formatType;

uint8_t sys_write(char * string, formatType format);
uint8_t sys_read(char * c);
uint8_t sys_exec(uint8_t process_c, void ** process_v);
uint8_t sys_exit(uint8_t err_code);


typedef void (* programs) ();
programs programs_list[] = {help,
                            zero_division_exc,
                            invalid_opcode_exc,
                            inforeg,
                            printmem,
                            hora,
                            primos,
                            fibonacci};

#endif //TPE_LIBC_H
