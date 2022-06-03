#include <libc.h>

static const char * Names[COUNT_REGS] = { "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "};

const time_func time_arr[] = {get_secs, 0, get_min, 0, get_hour, 0, get_day_week, get_day, get_month, get_year};

void help(uint64_t arg_c, const char ** arg_v){
    extern front_program_t programs[CANT_PROG];
    if(arg_c!=0){
        throw_error("Error: el programa no recibe argumentos");
    }
    print_string("Programas disponibles:\n", WHITE);
    for(int i = 0; i<CANT_PROG;i++){
        print_string(programs[i].desc,WHITE);
    }
//    print_string("\thelp: Despliega los distintos comandos disponibles\n", WHITE);
//    print_string("\tdiv0: Genera una excepcion por division por cero\n", WHITE);
//    print_string("\topcode: Genera una excepcion por division por cero\n", WHITE);
//    print_string("\tprintmem: Devuelve el vuelco de memoria de un puntero\n", WHITE);
//    print_string("\ttiempo: Fecha y hora actuales\n", WHITE);
//    print_string("\tinforeg: Imprime los registros del proceso\n",WHITE);
//    print_string("\tprimos: Despliega los numeros primos a partir del 1\n", WHITE);
//    print_string("\tfibonacci: Despliega los numeros de la serie de Fibonacci\n", WHITE);
    sys_exit();
}


void inforeg(uint64_t arg_c, const char ** arg_v){
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
        throw_error("Error: El programa no recibe argumentos");
    }
    print_string("Fecha y hora local (GMT-3): \n", WHITE);
    char * week[] = {" ", "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
    int day_months[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    print_string("\t", WHITE);
    int hs = time_arr[HOUR]();
    int day = time_arr[DAY_WEEK]();
    int day_month = time_arr[DAY_MONTH]();
    int month = time_arr[MONTH]();
    int year = time_arr[YEAR]();
//    int hs = 1;
//    int day = 1;
//    int day_month = 1;
//    int month = 1;
//    int year = 2022;
    if(hs < 3){                                 // Caso particular: Cuando en Greenwich es el dia siguiente al de Argentina
        year = (day == 1 && month==1)? year-1: year;
        if(((year % 4 == 0)&&( year % 100 != 0)) || (year % 400== 0))
            day_months[2] = 29;
        month = (day == 1)? ((month == 1)? 12 : month-1) : month;
        day_month = (day_month == 1)? day_months[month] : day_month-1;
        day = (day == 1)? 7 : day-1;
        hs = 24 + hs;
    }
    print_string(week[day], WHITE);
    print_string(" ", WHITE);
    print_number(day_month, WHITE);
    print_string("/", WHITE);
    print_number(month, WHITE);
    print_string("/", WHITE);
    print_number(year, WHITE);
    print_string(" ", WHITE);
    print_number(hs - 3, WHITE);
    print_string(":", WHITE);
    print_number(time_arr[MIN](), WHITE);
    print_string(":", WHITE);
    print_number(time_arr[SEC](), WHITE);
    print_string("hs\n", WHITE);
    sys_exit();
}


