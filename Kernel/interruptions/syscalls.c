#include <syscalls.h>
#include <queue.h>
uint8_t read_handler(char* str){
//    extern char buffer[];           // Obtenemos el buffer y los punteros de keyboard.c
//    extern int read;
//    extern int write;
    extern queue_t queue;
    if (is_empty(&queue)){
        *str = '\0'; //no hay caracteres para imprimir
        return 0;
    }          // No hay para leer (el puntero de escritura y lectura estan en la misma posicion), por lo que se devuelve 0
    *str = dequeue(&queue);           // Si hay para leer, guardamos el siguiente caracter en la primera posicion del string
    str[1] = '\0';
//    buffer[read] = '\0';
//    read = (read == BUFF_LENGTH-1) ? 0 : read+1;    // Movemos el puntero de lectura a la siguiente posicion
    return 1;                       // Devolvemos la cantidad de caracteres leidos
}

uint8_t write_handler(const char * str, formatType format){
    extern uint8_t process_array_len;
    if(process_array_len==0){
        //No se cargaron procesos, por default imprime en LEFT
        positionType position = LEFT;
        print(str, format, position);
    }
    print(str, format, get_current_position());        // Imprime por pantalla
    return 0; //para que no me tire warning por ahora

}

//TODO: que cant sea un enum
uint8_t exec_handler(uint8_t cant, void ** programs){
    if(cant == 0 || cant > 2)
        return -1;
    else if(cant==1){
        add_process(programs[0],ALL);
    }else{
        add_process(programs[0],LEFT);
        add_process(programs[1],RIGHT);
        print_lines();
    }
    return 0;
}

uint8_t exit_handler(){
    return terminate_process();
}

uint8_t time_handler(timeType time_unit){
    return get_time(time_unit);
}
