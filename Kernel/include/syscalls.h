#ifndef TPE_SYSCALLS_H
#define TPE_SYSCALLS_H

#include <video_driver.h>
#include <stdint.h>

int read_handler(char * str);
int write_handler(char* str);
int exec_handler(void* program);
int exit_handler(int code);

#endif
