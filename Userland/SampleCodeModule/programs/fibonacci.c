#include <fibonacci.h>

void fibonacci(void){
    uint64_t num_1 = 0;
    uint64_t num_2 = 1;
    print_number(num_1, WHITE);
    print_string("\n", WHITE);
    print_number(num_2, WHITE);
    print_string("\n", WHITE);
    while(1){
        uint64_t aux = num_1 + num_2;
        num_1 = num_2;
        num_2 = aux;
        print_number(aux, WHITE);
        print_string("\n", WHITE);
    }
    sys_exit(0);
}