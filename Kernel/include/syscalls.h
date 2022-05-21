#ifndef TPE_SYSCALLS_H
#define TPE_SYSCALLS_H

#include <video_driver.h>
#include <stdint.h>
#include <keyboard.h>
int read_handler(char * str);
int write_handler(char* str, int format);
int exec_handler(void* program);
int exit_handler(int code);

#endif
