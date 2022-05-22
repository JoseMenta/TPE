#ifndef TPE_VIDEO_DRIVER_H
#define TPE_VIDEO_DRIVER_H

#include <stdint.h>
#include <scheduler.h>

/*
#define BLACK       0x00
#define BLUE        0x01
#define GREEN       0x02
#define CYAN        0x03
#define RED         0x04
#define MAGENTA     0x05
#define BROWN       0x06
#define LIGHT_GRAY  0x07
#define DARK_GREY   0x08
#define LIGHT_BLUE  0x09
#define LIGHT_GREEN 0x0a
#define LIGHT_CYAN  0x0b
#define LIGHT_RED   0x0c
#define PINK        0x0d
#define YELLOW      0x0e
#define WHITE       0x0f
*/

typedef enum {BLACK=0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, PINK, YELLOW, WHITE} formatType;


void print(char * str, formatType letterFormat, positionType position);
void new_line(positionType position);
void println(char * str, formatType letterFormat, positionType position);
void scroll_up(positionType position);
void print_char(char c, formatType letterFormat, positionType position);
void print_dec_format(uint64_t value, positionType position);
void print_hex_format(uint64_t value, positionType position);
void print_bin_format(uint64_t value, positionType position);
void clear(positionType position);
void print_lines();
void delete_last_char(positionType position);


#endif
