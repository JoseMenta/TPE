#include <syscalls.h>

uint8_t read_handler(char* str){
    extern char buffer[];           // Obtenemos el buffer y los punteros de keyboard.c
    extern int read;
    extern int write;

    if (read == write)
        return 0;                   // No hay para leer (el puntero de escritura y lectura estan en la misma posicion), por lo que se devuelve 0
    *str = buffer[read];            // Si hay para leer, guardamos el siguiente caracter en la primera posicion del string
    str[1] = '\0';
    read = (read == BUFF_LENGTH-1) ? 0 : read+1;    // Movemos el puntero de lectura a la siguiente posicion
    return 1;                       // Devolvemos la cantidad de caracteres leidos
}

uint8_t write_handler(char * str, formatType format){

//    print(str, format, get_current_position());        // Imprime por pantalla
    positionType position = LEFT;
    print(str, format, position);
}

//TODO: que cant sea un enum
uint8_t exec_handler(uint8_t cant, void ** programs){
    if(cant == 0 || cant > 2)
        return -1;
    if(cant==1){
        add_process(programs[0],ALL);
    }else{
        add_process(programs[0],LEFT);
        add_process(programs[1],RIGHT);
    }
    return 0;
}

uint8_t exit_handler(){
    return terminate_process();
    // finishProcess();
}
