#include <libc.h>

void help(){
    print_string("Programas disponibles:\n", WHITE);
    print_string("    help: Despliega los distintos comandos disponibles\n", WHITE);
    print_string("    div0: Genera una excepcion por division por cero\n", WHITE);
    print_string("    invalid opcode: Genera una excepcion por division por cero\n", WHITE);
    print_string("    printmem: Devuelve el vuelco de memoria de un puntero\n", WHITE);
    print_string("    hora: Fecha y hora actuales\n", WHITE);
    print_string("    primos: Despliega los numeros primos a partir del 1\n", WHITE);
    print_string("    fibonacci: Despliega los numeros de la serie de Fibonacci\n", WHITE);
}

//void zero_division_exc(){} esta en asm->libasm.asm

//void invalid_opcode_exc(){} esta en asm->libasm.asm

void inforeg(){
    return;
}

void printmem(){
    return;
}

void hora(){
    return;
}

void primos(){
//    for(int i=0; ; i++){
//        if(is_prime()){
//            print_number(i, WHITE);
//            print_string(" ", WHITE);
//        }
//    }
}

void fibonacci(){
    return;
}

