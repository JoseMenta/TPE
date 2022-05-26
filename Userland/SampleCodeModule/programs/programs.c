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

//-----------------------------------------------------------------------
// hora: imprime fecha y hora local
//-----------------------------------------------------------------------
// Argumentos:
//
//-----------------------------------------------------------------------
// imprime:
//      Fecha y hora local:
//          Miercoles 25/05/2022 15:35hs
//-----------------------------------------------------------------------
void hora() {
    /*
       print_string("Fecha y hora local: \n", WHITE);
       Char ** week = {"lunes", "martes", "miercoles", "jueves", "viernes", "sabado", "domingo"};
       print_string("    ", wHITE);
       print_string(week[get_week()], WHITE);
       print_string(" ");
       print_number(get_day(), WHITE);
       print_string("/", WHITE);
       print_number(get_month(), WHITE);
       print_string("/", WHITE);
       print_number(get_year(), WHITE);
       print_string(" ", WHITE);
       print_number(get_hour(), WHITE);
       print_string(":", WHITE);
       print_number(get_min(), WHITE);
       print_string("hs\n", WHIET);
       return;

   */
}

