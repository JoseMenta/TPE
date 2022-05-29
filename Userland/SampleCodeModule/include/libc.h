
#ifndef TPE_LIBC_H
#define TPE_LIBC_H

#include <stdint.h>
#include <programs.h>

typedef void (* programs) ();

typedef enum {BLACK=0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, PINK, YELLOW, WHITE} formatType;

typedef enum {SEC = 0, MIN = 2, HOUR = 4, DAY_WEEK = 6, DAY_MONTH = 7, MONTH = 8, YEAR = 9} timeType;

#define NULL (0)
#define CANT_PROG (8)

uint8_t sys_write(const char * string, formatType format);
uint8_t sys_read(char * c);
uint8_t sys_exec(uint8_t process_c, void ** process_v);
uint8_t sys_exit();
uint8_t sys_time(timeType time_unit);
uint8_t sys_mem(uint64_t init_dir, uint8_t * arr);

uint64_t get_program(const char * str);
uint8_t get_char(void);
uint8_t print_string(const char * s1, formatType format);
uint8_t print_number(uint64_t number, formatType format);
uint64_t strcmp(const char * s1, const char * s2);
char * to_hex(char * str, uint64_t val);
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

uint8_t get_year(void);
uint8_t get_month(void);
uint8_t get_day_week(void);
uint8_t get_day(void);
uint8_t get_hour(void);
uint8_t get_min(void);
uint8_t get_secs(void);

uint64_t* get_registers(void);
uint64_t get_register(void);
uint64_t get_memory(uint32_t* pointer);
int str_tok(char * buffer, char sep);

#endif //TPE_LIBC_H
