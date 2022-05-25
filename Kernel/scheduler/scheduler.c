#include <scheduler.h>
#include <stdint.h>

process_t process_array[5] = {{{0}}};
uint8_t process_array_len = 0;
uint8_t currentProcess_index = 0;

void setup_context(uint64_t* context,void* new_rip, uint64_t new_rsp);
void change_context();

uint64_t * getCurrContext(void);

//-----------------------------------------------------------------------
// add_process: Agrega un proceso que se maneja en toda la pantalla
//-----------------------------------------------------------------------
// Argumentos:
// - start_position: la direccion de memoria donde comienza el proceso
//-----------------------------------------------------------------------
void add_process(void * start_position){
    uint8_t running_process = 0;
    for(int i = 0; i < process_array_len; i++){
        if(process_array[i].status == RUNNING){
            running_process = i;
        }
    }
    uint64_t * curr_context = getCurrContext();
//    currentProcess_index = process_array_len;
    setup_context(process_array[process_array_len].registers,start_position,curr_context[RSP]-(process_array_len+1)*OFFSET);
    process_array[process_array_len].position = ALL;
    process_array[process_array_len++].status = RUNNING;
}


//-----------------------------------------------------------------------
// add_left_right_process: Agrega un proceso en left y otro en right
//-----------------------------------------------------------------------
// Argumentos:
// - left_start: la direccion de memoria donde comienza el programa en left
// - left_start: la direccion de memoria donde comienza el programa en right
//-----------------------------------------------------------------------
void add_left_right_process(void* left_start, void* right_start){
    //pauso a todos los procesos anteriores
    for(int i = 0; i < process_array_len; i++){
        if(process_array[i].status == RUNNING){
            process_array[i].status = PAUSED;
        }
    }
    uint64_t * curr_context = getCurrContext();
    setup_context(process_array[process_array_len].registers,left_start,curr_context[RSP]-(process_array_len+1)*OFFSET);
    process_array[process_array_len].position = ALL;
    process_array[process_array_len++].status = RUNNING;

    // Tiene que cambiar el arreglo curr_context
    setup_context(process_array[process_array_len].registers,right_start,curr_context[RSP]-(process_array_len+1)*OFFSET);
    process_array[process_array_len].position = ALL;
    process_array[process_array_len++].status = RUNNING;
    //rsp1 == rsp+1000
    //rsp2 == rsp+2000
}


//-----------------------------------------------------------------------
// setup_context: Inicializa los registros para el nuevo proceso
//-----------------------------------------------------------------------
// Argumentos:
// - context: el arreglo donde se almacenan los registros del proceso
// - new_rip: la direccion de memoria donde comienza el proceso
// - new_rsp: la direccion de memoria donde comienza el stack para el proceso
//-----------------------------------------------------------------------
void setup_context(uint64_t* context, void * new_rip, uint64_t new_rsp){
    // Inicializa los registros en 0 para el nuevo programa
    for(int  i = 0; i<REGISTERS_COUNT; i++){
        context[i] = 0;
    }
    // Se indica la direccion de memoria donde comienza el proceso (programa)
    context[RIP] = (uint64_t) new_rip;
    // Se indica la direccion de memoria donde comienza el stack local al proceso
    context[RSP] = new_rsp;
}


//-----------------------------------------------------------------------
// change_context: Cambia los registros cuando cambio los programas
//-----------------------------------------------------------------------
// Argumentos:
//-----------------------------------------------------------------------
void change_context(){
    //Si no hay procesos corriendo (en el primer llamado):
    //  solo pasar el nuevo contexto que esta en la estructura en los registros
    //  poner el estado en RUNNING
    //  return
    //Si hay procesos corriendo:
    //  Guardar el contexto actual (que saco con el getter de asm) en la estructura
    //  No cambiamos el estado del proceso que sacamos, pues sigue corriendo
    //  ver cual es el siguiente proceso a ejecutar (que es el siguiente en estado running)
    //  Cargar el contexto del siguiente proceso
    //  return
    return;
}


