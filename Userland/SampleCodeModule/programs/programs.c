//#include <libc.h>

extern int sys_write(char* string, int format);
extern int sys_read(char* c);

void help(){
    sys_write("help: despliega los distintos comandos disponibles");
    sys_write("div0: genera una excepcion por division por cero");
    sys_write("invalid opcode: genera una excepcion por division por cero");
}

//void zero_division_exc(){} esta en asm->libasm.asm

//void invalid_opcode_exc(){} esta en asm->libasm.asm

void inforeg(){}

void printmem(){}

void hora(){}

void primos(){}

void fibonacci(){};

