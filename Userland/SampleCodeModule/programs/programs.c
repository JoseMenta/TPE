#include <libc.h>
#include <math.h>

static const char * Names[COUNT_REGS] = { "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "};

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
void to_hex(char * str, uint64_t val);
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
uint64_t get_memory(uint32_t* pointer);


void inforeg(){
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
}

//-----------------------------------------------------------------------
// uintToBase: Convierte un entero en la base indica por parametro en un string
//-----------------------------------------------------------------------
// Argumentos:
//  value: el valor del entero
//  buffer: el string sobre cual copiar
//  base: la base a convertir del entero
//-----------------------------------------------------------------------
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    //Calculate characters for each digit
    do
    {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    }
    while (value /= base);

    // Terminate string in buffer.	(El \0 del string)
    *p = 0;

    //Reverse string in buffer. (Notar que al hacer el pasaje de int a char, se tiene el numero al reves pues se analiza de derecha a izquierda)
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

//-----------------------------------------------------------------------
// to_hex: Devuelve un entero hexadecimal en un string
//-----------------------------------------------------------------------
// Argumentos:
//  str: el string sobre cual copiar
//  val: el valor del entero
//-----------------------------------------------------------------------
void to_hex(char * str, uint64_t val){
    uintToBase(val,str,16);
}

void printmem(){}

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
       print_string("Fecha y hora local: \n", WHITE);
       char * week[] = {"lunes", "martes", "miercoles", "jueves", "viernes", "sabado", "domingo"};
       print_string("    ", WHITE);
       print_string(week[get_week()], WHITE);
       print_string(" ", WHITE);
       print_number(get_day(), WHITE);
       print_string("/", WHITE);
       print_number(get_month(), WHITE);
       print_string("/", WHITE);
       print_number(get_year(), WHITE);
       print_string(" ", WHITE);
       print_number(get_hour()-3, WHITE);
       print_string(":", WHITE);
       print_number(get_min(), WHITE);
       print_string("hs\n", WHITE);
       return;
}

