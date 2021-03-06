#include <syscalls.h>
#include <video_driver.h>
#include <keyboard.h>
#include <scheduler.h>
#include <queue.h>
#include <time.h>
//----------------------------------------------------------------------
// read_handler: handler para leer un caracter del teclado
//----------------------------------------------------------------------
// Argumentos:
//  str: char* donde se debe guardar el caracter
//----------------------------------------------------------------------
// Retorno:
//  0 si no hay caracteres para ller
//  1 si hay caracteres para leer
//----------------------------------------------------------------------
uint8_t read_handler(char* str){
    extern queue_t queue;
    if (is_empty(&queue)){
        *str = '\0'; //no hay caracteres para imprimir
        return 0;
    }          // No hay para leer (el puntero de escritura y lectura estan en la misma posicion), por lo que se devuelve 0
    *str = dequeue(&queue);           // Si hay para leer, guardamos el siguiente caracter en la primera posicion del string
    str[1] = '\0';
    return 1;                       // Devolvemos la cantidad de caracteres leidos
}
//----------------------------------------------------------------------
// write_handler: imprime un string en la pantalla del proceso que lo llama
//----------------------------------------------------------------------
// Argumentos:
//  str: el string que se desea imprimor
//  format: el color de la letra que se desea usar
//----------------------------------------------------------------------
uint8_t write_handler(const char * str, formatType format){
    if(process_array_is_empty()){
        //No se cargaron procesos, por default imprime en LEFT
        positionType position = ALL;
        print(str, format, position);
        return 0;
    }
    print(str, format, get_current_position());        // Imprime por pantalla
    return 0;
}

//----------------------------------------------------------------------
// ecec_handler: agrega procesos para que se ejecuten
//----------------------------------------------------------------------
// Argumentos:
//  cant: la cantidad de procesos que se desea ejecutar (1 si es en toda la pantalla, 2 si es en pantalal dividida)
//  programs: arreglo de program_t con los programas que se desean ejecutar
//----------------------------------------------------------------------
// Si cant==1, programs debe tener al menos 1 elemento, y se ejecuta el primero
// Si cant==2, programs debe tener al menos 2 elementos, el primero se ejecuta a la izquierda y el
//  segundo a la derecha
//----------------------------------------------------------------------
// Retorno:
//  -1 si cant no es valido
//  0 si logro ejecutar a los procesos
//----------------------------------------------------------------------
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
//----------------------------------------------------------------------
// exit_handler: termina el proceso que lo llama
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Retorno
//  -1 si no hay procesos para terminar
//  0 si logra terminar el proceso
//----------------------------------------------------------------------
uint8_t exit_handler(){
    return terminate_process();
}
//----------------------------------------------------------------------
// time_handler: obtiene la unidad del tiempo que se pide
//----------------------------------------------------------------------
// Argumentos:
//  time_unit: la unidad que se desea buscar (sec, min,....)
//----------------------------------------------------------------------
// Retorno:
//  El valor pedido o -1 si no es un parametro correcto
//----------------------------------------------------------------------
int8_t time_handler(timeType time_unit){
    if(time_unit != SEC && time_unit != MIN && time_unit != HOUR && time_unit != DAY_WEEK &&
        time_unit != DAY_MONTH && time_unit != MONTH && time_unit != YEAR){
        return -1;
    }
    return get_time(time_unit);
}
//----------------------------------------------------------------------
// mem_handler: guarda 32 bytes a partir de una direccion que se pasa en un vector
//----------------------------------------------------------------------
// Argumentos:
//  init_dir: la direccion inicial desde donde se quiere leer
//  arr: arreglo donde se escriben los valores, debe tener al menos 32 posiciones
//----------------------------------------------------------------------
// Es la funcion que utliza el programa printmem. Completa con 0's si en algun momento
// se pasa del maximo
//----------------------------------------------------------------------
// Retorno:
//  La cantidad de posiciones que se logro almacenar
//----------------------------------------------------------------------
uint8_t mem_handler(uint64_t init_dir, uint8_t * arr){
    uint8_t i = 0;
    // Empiezo a completar el arreglo, siempre y cuando la direccion consultada sea menor a la ultima
    // Asi se evita overflow
    for(; (i < MAX_ARRAY_SIZE) && (init_dir + i < MAX_MEM_ADDRESS); i++){
//        arr[i] = get_data(init_dir + i);
        arr[i] = *((uint8_t*)init_dir + i);//char* para que +i avance de a 1
    }
    if(init_dir + i == MAX_MEM_ADDRESS){
//        arr[i++] = get_data(MAX_MEM_ADDRESS);
        arr[i++] = *((uint8_t*)MAX_MEM_ADDRESS);
    }
    // Si aun quedan espacios libres (i < 32), se completa con NULL
    for(int j = i; j < MAX_ARRAY_SIZE; j++){
        arr[j] = NULL;
    }
    // En i se almacenan la cantidad real de datos que se pudieron almacenar
    return i;
}
//----------------------------------------------------------------------
// tick_handler: obtiene la cantidad de ticks que hizo el timer tick
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Retorno
//  La cantidad de ticks
//----------------------------------------------------------------------
uint64_t tick_handler(void){
    return ticks_elapsed();
}
//----------------------------------------------------------------------
// blink_handler: hace que el fondo del cursor de la pantalla cambie segun lo especificado en video_blink
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
uint8_t blink_handler(void){
    video_blink(get_current_position());
    return 0;
}
//----------------------------------------------------------------------
// regs_handler: devuelve los valores de los arreglos cuando se utilizo la combinacion Control+s
//----------------------------------------------------------------------
// Argumentos:
//  regs_arr: un arreglo de al menos 18 uint64_t donde se almacenan los valores
//----------------------------------------------------------------------
uint8_t regs_handler(uint64_t * regs_arr){
    extern uint8_t regs_saved;
    uint64_t * inforeg_context = get_inforeg_context();
    for(uint8_t i = 0; i < REGISTERS_COUNT; i++){
        regs_arr[i] = inforeg_context[i];
    }
    return regs_saved;
}
//----------------------------------------------------------------------
// clear_handler: Limpiar la terminal de comandos
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
uint8_t clear_handler(void){
    clear(ALL);
    return 0;
}