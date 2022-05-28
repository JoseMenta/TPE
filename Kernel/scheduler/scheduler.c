#include <scheduler.h>
#include <stdint.h>
#include <stdarg.h>
#include <video_driver.h>
static process_t process_array[5] = {{{0}}};
uint8_t process_array_len = 0;
uint8_t currentProcess_index = 0; //Arranca en 0, el proceso default


//TODO: Agregar un flag para ver si el usuario queria salir del programa, y recien ahi restaurar a los waiting
//El arreglo va a estar compuesto por
//[main_userland,bash,...(procesos que llama el bash)]
//Si la cantidad de procesos es 4 => se estan corriendo pantallas divididas
//Si es 3 => se esta corriendo solo un proceso encima de la terminal
//Si es 2 => se esta corriendo solo la terminal
//Si es 1 => Esta en userland
//Dejamos main_userland para poder terminar con el kernel sin tener que forzar la salida
void setup_context(uint64_t* context,void* new_rip, uint64_t new_rsp);
void change_context();
void copy_context(uint64_t* source, uint64_t* dest);
void default_process();
//Implementacion alternativa
uint8_t want_to_return = 0; //Flag para ver si el usuario desea volver al proceso anterior al que se esta corriendo luego de que el ultimo haya terminado
static int8_t left_index = -1;
static int8_t right_index = -1;
static int8_t full_index = -1;
static uint8_t runnable = 0;
void add_full_process(void* process_start);
static int add_process_to_array(void* process_start, positionType position);
static void add_original_process();
static void switch_context(uint8_t new_index);
//Fin implementacion alternativa

positionType get_current_position(){
    return process_array[currentProcess_index].position;
}

uint64_t * getCurrContext(void); //funcion auxiliar, devuelve un vector que fue declarado en interrupts.asm


//Lo siguiente es una implementacion alternativa de la logica del scheduler
void add_process(void * process_start, positionType position){
    add_full_process(process_start);
}

void change_context(){
    if(process_array_len==0) return; //No hay procesos disponibles para cambiar
    uint64_t* curr_context_arr = getCurrContext();
    if(runnable==0 || want_to_return){
        //Terminaron todos los procesos que se podian correr
        if(!want_to_return){
            curr_context_arr[RIP] = (uint64_t) &default_process; //Me quedo esperando a que quiera volver (voy a estar en el contexto de uno de los que estaba corriendo)
            return;
        }
        want_to_return = 0; //Lo seteo para el proximo uso
        int i = process_array_len-1;
        for(; process_array[i].status != WAITING; i--); //Busco el index del ultimo proceso WAITIN
        process_array[i].status=RUNNING;
        runnable++;
        full_index = i; //El nuevo proceso en la pantalla completa es el
        clear(process_array[i].position);//Limplia la pantalla para el proceso que vuelve
        switch_context(i);//Cambio el contexto al de ese proceso
        process_array_len = i +1;//Elimino a los procesos que venian despues de el
        return;
    }else if(runnable == 2){
        if(process_array[left_index].status==SUSPENDED && process_array[right_index].status==SUSPENDED){
        //Si ambos estan suspendidos, corro el proceso default
            curr_context_arr[RIP] = (uint64_t) &default_process;
            return;
        }
        if(currentProcess_index==left_index){
            //Si esta corriendo el proceso de la izquierda
            if(process_array[right_index].status!=RUNNING){
                //Si el proceso de la derecha termino o esta suspendido
                copy_context(process_array[left_index].registers,curr_context_arr);
                return; //Me quedo en el proceso de la izquierda
            }else{
                //Cambio para que siga el proceso de la derecha
                switch_context(right_index);
                return;
            }
        }else{
            // Esta corriendo el proceso de la derecha
            if (process_array[left_index].status!=RUNNING) {
                // No hace nada: sigue con el proceso de la derecha
                copy_context(process_array[right_index].registers,curr_context_arr);
                return;
            } else {
                //Cambio para que siga el proceso de la izquierda
                switch_context(left_index);
            }
        }
    }else if (runnable==1){
        //Solo tengo uno
        //O tenia 2 y uno termino
        //Puedo llegar aca cuando termina un proceso y tiene que seguir el otro
        if(process_array[currentProcess_index].status==TERMINATED){
            //Si el que estaba corriendo termino
            //Tengo que ir al otro
            int new_index = (currentProcess_index==left_index)?right_index:left_index; //Elijo el que no termino ahora
            if(currentProcess_index == left_index) left_index = -1;//Indico que ya no esta corriendo
            if(currentProcess_index == right_index) right_index = -1;
            switch_context(new_index);
        }else if(process_array[currentProcess_index].status==SUSPENDED){
            curr_context_arr[RIP] = (uint64_t) &default_process;
        }else {
            copy_context(process_array[currentProcess_index].registers,curr_context_arr);
        }
        //Si no, sigo corriendo el de ahora
        return;
    }
    return;
}

uint8_t terminate_process(){
    if(process_array_len==1) {
        return -1;//No puedo terminar el proceso raiz
    }
    process_array[currentProcess_index].status = TERMINATED;
    //if(currentProcess_index==left_index) left_index=-1;
    //if(currentProcess_index==right_index) right_index=-1;
    runnable--; //OJO: importa que este aca arriba, si no cambia la logica de runnable de change_context
    change_context();                                           // Tengo que ver cual es el proximo que corro
    return 0;
}

// Teclas especiales

uint8_t suspend_left(){
    uint64_t* curr_context_arr = getCurrContext();
    if(left_index==-1 || process_array[left_index].status==TERMINATED) return 1;
    process_array[left_index].status = SUSPENDED;
    copy_context(curr_context_arr,process_array[left_index].registers);
    change_context();
    return 0;
}

uint8_t suspend_right(){
    uint64_t* curr_context_arr = getCurrContext();
    if(right_index==-1 || process_array[right_index].status==TERMINATED) return 1;
    process_array[right_index].status = SUSPENDED;
    copy_context(curr_context_arr,process_array[right_index].registers);
    change_context();
    return 0;
}

uint8_t suspend_full(){
    uint64_t* curr_context_arr = getCurrContext();
    if(full_index==-1 || process_array[full_index].status==TERMINATED) return 1;
    process_array[full_index].status = SUSPENDED;
    copy_context(curr_context_arr,process_array[full_index].registers);
    change_context();
    return 0;
}

uint8_t restart_left(){
    if(left_index==-1 || process_array[left_index].status==TERMINATED) return 1;
    process_array[left_index].status = RUNNING;
    change_context();
    return 1;
}

uint8_t restart_right(){
    if(right_index==-1 || process_array[right_index].status==TERMINATED) return 1;
    process_array[right_index].status = RUNNING;
    change_context();
    return 1;
}

uint8_t restart_full(){
    if(full_index==-1 || process_array[full_index].status==TERMINATED) return 1;
    process_array[full_index].status = RUNNING;
    change_context();
    return 1;
}

void add_two_processes(void* left_start, void* right_start){
    if(process_array_len==0){
        add_original_process();
    }else{
        runnable--;
    }
    process_array[currentProcess_index].status = WAITING;
    left_index = add_process_to_array(left_start,LEFT);
    right_index = add_process_to_array(right_start,RIGHT);
    switch_context(right_index); //Cambia el contexto como para que primero se ejecute el programa de la izquierda
}

void add_full_process(void* process_start){
    if(process_array_len==0){ //si es el primer proceso, agrego a quien lo ejecuto tambien para poder volver
        add_original_process();
    }else{
        runnable--; //Deja de correr el que lo llama (bash)
    }
    process_array[currentProcess_index].status = WAITING; //Seteo al proceso actual para que espera hasta que termine el nuevo proceso
    full_index = add_process_to_array(process_start,ALL);
    switch_context(full_index); //Cambia el contexto para que se comience a ejecutar el nuevo programa
}

//Agrega el proceso original, que es el primer proceso que llama a sys_exec
static void add_original_process(){

    uint64_t* curr_context = getCurrContext();
    copy_context(curr_context,process_array[process_array_len].registers);
    process_array[process_array_len].position = ALL;
    process_array[process_array_len++].status = WAITING; //Espera a que termine el proceso que lo llamo
    //No puedo hacer esto porque cuando hace process_array[0].registers, va a buscar basura
}

//Agrega un proceso al final del arreglo de procesos (en estado RUNNING)
//Devuelve el index del proceso que se agrego
static int add_process_to_array(void* process_start, positionType position){
    //Inicializo en contexto del nuevo proceso
    setup_context(process_array[process_array_len].registers,process_start,process_array[0].registers[RSP]-(process_array_len)*OFFSET);
    process_array[process_array_len].position = position;
    process_array[process_array_len++].status = RUNNING;
    runnable++; //aumento el contador de procesos que pueden correr
    return process_array_len-1; //Deuvelve el index del proceso que se agrego
}
//Cambia el contexto para que pase del proceso en currentProcess_index al proceso en el index new_index
static void switch_context(uint8_t new_index){
    uint64_t* curr_context_arr = getCurrContext();
    if(process_array[currentProcess_index].status==RUNNING){
        //Solo guardo el contexto si no estaba en WAITING, si no no lo guardo para no cambiar los registros (en especial los flags)
        copy_context(curr_context_arr,process_array[currentProcess_index].registers); //Guardo el contexto actual en el proceso que se estaba corriendo
    }
    copy_context(process_array[new_index].registers,curr_context_arr);//Cargo el contexto del nuevo proceso para que se configure antes de iretq
    if(process_array[new_index].status==SUSPENDED){
        //Si esta suspendido, cambio el RIP para que en su lugar se ejecute el default_process
        curr_context_arr[RIP] = (uint64_t) &default_process;
    }
    currentProcess_index = new_index; //Indico que se comienza a ejecutar ese proceso
}
//Funciones auxiliares de la implementacion
void setup_context(uint64_t * context, void * new_rip, uint64_t new_rsp){
    // Inicializa los registros en 0 para el nuevo programa
    for(int  i = 0; i<REGISTERS_COUNT; i++){
        context[i] = 0;
    }
    // Se indica la direccion de memoria donde comienza el proceso (programa)
    context[RIP] = (uint64_t) new_rip;
    // Se indica la direccion de memoria donde comienza el stack local al proceso
    context[RSP] = new_rsp;
}
void copy_context(uint64_t* source, uint64_t* dest){
    for(int i = 0; i<REGISTERS_COUNT;i++){
        dest[i] = source[i];
    }
}
//Fin implementacion alternativa