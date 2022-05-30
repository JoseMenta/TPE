#include <syscalls.h>

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
        positionType position = ALL;
        print(str, format, position);
        return 0;
    }
    print(str, format, get_current_position());        // Imprime por pantalla
    return 0; //para que no me tire warning por ahora

}

//TODO: que cant sea un enum
uint8_t exec_handler(uint8_t cant, const program_t* programs){//Recibe un vector de program_t
    if(cant == 0 || cant > 2)
        return -1;
    else if(cant==1){
        clear(ALL);
        add_full_process(programs[0]);
        //add_process(programs[0],ALL);
    }else{
        clear(ALL);//Limpio la pantalla y reinicio las posiciones de ambas subpantallas
        add_two_processes(programs[0],programs[1]);
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

// TODO: Preguntar a horacio si lanzamos error si se supera el limite de memoria
uint8_t mem_handler(uint64_t init_dir, uint8_t * arr){
    uint8_t i = 0;
    // Empiezo a completar el arreglo, siempre y cuando la direccion consultada sea menor a la ultima
    // Asi se evita overflow
    for(; (i < MAX_ARRAY_SIZE) && (init_dir + i < MAX_MEM_ADDRESS); i++){
        arr[i] = get_data(init_dir + i);
    }
    // Si el for corto por el overflow, se guarda el dato almacenado en la ultima direccion
    if(init_dir + i == MAX_MEM_ADDRESS){
        arr[i++] = get_data(MAX_MEM_ADDRESS);
    }
    // Si aun quedan espacios libres (i < 32), se completa con NULL
    for(int j = i; j < MAX_ARRAY_SIZE; j++){
        arr[j] = NULL;
    }
    // En i se almacenan la cantidad real de datos que se pudieron almacenar
    return i;
}
