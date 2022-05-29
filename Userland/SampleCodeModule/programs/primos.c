#include <primos.h>

uint8_t is_prime(uint64_t number);

uint8_t is_prime(uint64_t number){
    for(int i=FIRST_PRIME; i*i <= number; i++){
        if(number%i == 0)
            return 0;
    }
    return 1;
}

void primos(){
   for(uint64_t i=FIRST_PRIME; 1 ; i++){
        if(is_prime(i)){
           print_number(i, WHITE);
           print_string("\n", WHITE);
        }
   }
   sys_exit(0);
}


