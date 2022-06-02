#include <primos.h>

static uint8_t is_prime(uint64_t number){
    for(int i=FIRST_PRIME; i*i <= number; i++){
        if(number%i == 0)
            return 0;
    }
    return 1;
}

void primos(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=0){
        throw_error("Error: el programa no recibe argumentos");
    }
   for(uint64_t i=FIRST_PRIME; 1 ; i++){
        if(is_prime(i)){
           pause_ticks(5);
           print_number(i, WHITE);
           print_string("\n", WHITE);
        }
   }
   sys_exit();
}


