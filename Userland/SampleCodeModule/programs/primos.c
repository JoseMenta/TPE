#include <math.h>
#include <libc.h>


int is_prime(int number){
    for(int i=2; i <= number/2; i++){
        if(number%i ==0)
            return 0;
    }
    return 1;
}

void primos(){
   for(int i=0; 1 ; i++){
        if(is_prime(i)){
           print_number(i, WHITE);
           print_string("\n", WHITE);
        }
   }
   sys_exit(0);
}


