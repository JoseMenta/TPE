/* sampleCodeModule.c */
#include <libc.h>
#include <tests.h>
#include <programs.h>
#include <bash.h>



int main() {
    program_t aux[] = {{bash, 0, NULL}};
    sys_exec(1, aux);
    return 0;

}