#include <libc.h>

static const char * Names[COUNT_REGS] = { "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "};

const time_func time_arr[] = {get_secs, 0, get_min, 0, get_hour, 0, get_day_week, get_day, get_month, get_year};

void help(uint64_t arg_c, const char** arg_v){
    if(arg_c!=0){
        print_string("Error: el programa no recibe argumentos",STDERR);
        sys_exit();
    }
    print_string("Programas disponibles:\n", WHITE);
    print_string("\thelp: Despliega los distintos comandos disponibles\n", WHITE);
    print_string("\tdiv0: Genera una excepcion por division por cero\n", WHITE);
    print_string("\tinvalid opcode: Genera una excepcion por division por cero\n", WHITE);
    print_string("\tprintmem: Devuelve el vuelco de memoria de un puntero\n", WHITE);
    print_string("\thora: Fecha y hora actuales\n", WHITE);
    print_string("\tprimos: Despliega los numeros primos a partir del 1\n", WHITE);
    print_string("\tfibonacci: Despliega los numeros de la serie de Fibonacci\n", WHITE);
    sys_exit();
}


void inforeg(uint64_t arg_c, const char** arg_v){
    if(arg_c!=0){
        print_string("Error: El programa no recibe argumentos",STDERR);
        sys_exit();
    }
    print_string("Registros: \n", WHITE);
    // Pongo en reg los valores de los registros
    uint64_t * reg = get_registers();
    char reg_str[20];
    for(int i=0; i<COUNT_REGS; i++){
        print_string(Names[i], WHITE);
        to_hex(reg_str, reg[i]);
        print_string(reg_str,WHITE);
        print_string("\n", WHITE);
    }
    sys_exit();
}

//void printmem(uint64_t init_dir)
//limit = 0xFFFFFFFD9
void printmem(uint64_t arg_c, const char** arg_v){
    if(arg_c!=1){
        print_string("ERROR: El programa debe recibir unicamente 1 argumento", STDERR);
        sys_exit();
    }
    uint64_t init_dir = 0xFFFFFFFD9;
    uint8_t mem_arr[32] = {0};
    uint8_t dim = sys_mem(init_dir, mem_arr);

    char str[21] = {0};                                                             // 2^64 tiene 20 digitos mas el "\0"
    print_string("Datos almacenados a partir de la direccion 0x", WHITE);
    print_string(to_hex(str, init_dir), WHITE);
    print_string(":\n", WHITE);
    for(int i = 0; i < dim; i++){
        print_string("0x", WHITE);
        print_string(to_hex(str, init_dir + i), WHITE);
        print_string(": ", WHITE);
        print_string("0x", WHITE);
        print_string(to_hex(str, mem_arr[i]), WHITE);
        (i%2==0)? print_string("\t", WHITE): print_string("\n", WHITE);
    }
    print_string("\n", WHITE);
    sys_exit();
}

/*
void printmem(char * dir_memoria){                          // deberia ir como parametro de la funcion, cambia bastante la logica de analizebuffer
    if(dir_memoria[0] != '0' && dir_memoria[1] != 'x')
        print_string("Formato invalido, coloque la direccion en formato hexadecimal", WHITE);

    int len = 0;
    while(dir_memoria[len]!='\0')                           // retengo el largo del array
        len++;

    uint64_t val = 0;                                      // lo paso a entero para poder desreferenciar
    for(int i=len; i!=1; i--){
        char num = dir_memoria[i];
        if (num >= '0' && num <= '9')
            val += (num - '0') * (16 - len-i);
        else if (num >= 'A' && num <='F')
            val += (num - 'A' + 10) * (16 - len-i);
    }

    uint32_t* pointer = (uint32_t*) val;                  // casteo el valor a un puntero
    uint64_t result = get_memory(pointer);                 // obtengo un vuelco de 32bits

    char str[12];                                          // 2^32 tiene 10 digitos mas el "\0" y un posible "-"
    to_hex(str, result);
    print_string("Vuelco de memoria: ", WHITE);           // imprimo el valor en hexa nuevamente
    print_string(str, WHITE);
    return;
}
*/

//-----------------------------------------------------------------------
// tiempo: imprime fecha y hora local en tiempo GMT-3
//-----------------------------------------------------------------------
// Argumentos:
//  null
//-----------------------------------------------------------------------
// Imprime:
//      Fecha y hora local (GMT-3):
//          Miercoles 25/05/2022 15:35:20hs
//-----------------------------------------------------------------------
void tiempo(uint64_t arg_c, const char ** arg_v) {
    if(arg_c!=0){
        print_string("Error: El programa no recibe argumentos",STDERR);
        sys_exit();
    }
    print_string("Fecha y hora local (GMT-3): \n", WHITE);
    // TODO: Chequear que valor devuelve para cada dia
    char * week[] = {" ", "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
    print_string("    ", WHITE);
    print_string(week[time_arr[DAY_WEEK]()], WHITE);
    print_string(" ", WHITE);
    print_number(time_arr[DAY_MONTH](), WHITE);
    print_string("/", WHITE);
    print_number(time_arr[MONTH](), WHITE);
    print_string("/", WHITE);
    print_number(time_arr[YEAR](), WHITE);
    print_string(" ", WHITE);
    int hs = time_arr[HOUR]();
    if(hs < 3) {                          // Caso particular: Cuando en Greenwich es el dia siguiente al de Argentina
        hs = 24 + hs;
    }
    print_number(hs - 3, WHITE);
    print_string(":", WHITE);
    print_number(time_arr[MIN](), WHITE);
    print_string(":", WHITE);
    print_number(time_arr[SEC](), WHITE);
    print_string("hs\n", WHITE);
    sys_exit();
}

