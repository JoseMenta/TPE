#ifndef TPE_VIDEO_DRIVER_H
#define TPE_VIDEO_DRIVER_H

#include <stdint.h>
#include <scheduler.h>
#include <keyboard.h>
#include <time.h>

typedef enum {BLACK=0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, PINK, YELLOW, WHITE, BACKGROUND_BLACK = BLACK << 4, BACKGROUND_WHITE = WHITE << 4} formatType;

void print_tab(positionType position, formatType letterFormat);
void print(const char * str, formatType letterFormat, positionType position);
void new_line(positionType position);
void println(const char * str, formatType letterFormat, positionType position);
void scroll_up(positionType position);
void print_char(char c, formatType letterFormat, positionType position);
void print_dec_format(uint64_t value, positionType position);
void print_hex_format(uint64_t value, positionType position);
void print_bin_format(uint64_t value, positionType position);
void clear(positionType position);
void print_lines();
void delete_last_char(positionType position);
void video_blink(positionType position);

#endif
