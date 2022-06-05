#include <scheduler.h>
#include <interrupts.h>
#include <stdint.h>
#include <keyboard.h>
#include <video_driver.h>

#define ORIGINAL_PROCESS_INDEX 0
#define DEFAULT_PROCESS_INDEX 4
static process_t process_array[5] = {{{0}}};
uint8_t process_array_len = 0;
static uint8_t currentProcess_index = 0; //Arranca en 0, el proceso default


//TODO: Agregar un flag para ver si el usuario queria salir del programa, y recien ahi restaurar a los waiting
//El arreglo va a estar compuesto por
//[main_userland,bash,...(procesos que llama el bash)]
//Si la cantidad de procesos es 4 => se estan corriendo pantallas divididas
//Si es 3 => se esta corriendo solo un proceso encima de la terminal
//Si es 2 => se esta corriendo solo la terminal
//Si es 1 => Esta en userland
//Dejamos main_userland para poder terminar con el kernel sin tener que forzar la salida
void setup_context(uint64_t* context,program_t program, uint64_t new_rsp, uint64_t prev_actual_rflags);
void change_context();
void copy_context(uint64_t* source, uint64_t* dest);
void default_process();

uint8_t want_to_return = 0; //Flag para ver si el usuario desea volver al proceso anterior al que se esta corriendo (independientemente de si el ultimo termino o no)
static int8_t left_index = -1;
static int8_t right_index = -1;
static int8_t full_index = -1;
static uint8_t runnable = 0;

static int add_process_to_array(program_t process_start, positionType position);
static void add_original_process();
static void switch_context(uint8_t new_index);
static uint8_t terminate(int8_t index);

//Fin implementacion alternativa

positionType get_current_position(){
    return process_array[currentProcess_index].position;
}


void change_context(){
    //change_context cambia el contexto cuando llega un TT
    if(process_array_len==0) return;
    if(runnable==0 || want_to_return){
        //Terminaron todos los procesos que se podian correr
        if(!want_to_return){
            //corre el proceso default, se queda esperando a que toque esc
            switch_context(DEFAULT_PROCESS_INDEX); //Me quedo esperando a que quiera volver (voy a estar en el contexto de uno de los que estaba corriendo)
            return;
        }
        runnable = 0;
        want_to_return = 0; //Lo seteo para el proximo uso
        int i = process_array_len-1;
        for(; process_array[i].status != WAITING; i--); //Busco el index del ultimo proceso WAITING
        process_array[i].status=RUNNING;
        runnable++;
        left_index = -1;
        right_index = -1;
        full_index = i; //El nuevo proceso en la pantalla completa es el ultimo que se freno
        clear_keyboard_buffer();
        clear(process_array[i].position);//Limpia la pantalla para el proceso que vuelve
        switch_context(i);//Cambio el contexto al de ese proceso
        process_array_len = i +1;//Elimino a los procesos que venian despues de el
        return;
    }else if (runnable==2){
        if(process_array[left_index].status == SUSPENDED && process_array[right_index].status==SUSPENDED){
            switch_context(DEFAULT_PROCESS_INDEX);
            return;
        }
        if(currentProcess_index==left_index){
            if(process_array[right_index].status!=RUNNING){
                switch_context(left_index);
            }else{
                switch_context(right_index);
            }
        }else{
            if(process_array[left_index].status!=RUNNING){
                switch_context(right_index);
            }else{
                switch_context(left_index);
            }
        }
    }else if(runnable==1){
        if(process_array[currentProcess_index].status==TERMINATED){
            int next_index = (currentProcess_index==left_index)?right_index:left_index;
            switch_context(next_index);
        }else if(process_array[currentProcess_index].status==SUSPENDED){
            switch_context(DEFAULT_PROCESS_INDEX);
        }else{
            switch_context(currentProcess_index);
        }
        return;
    }
    return;
}

uint8_t terminate_process(){ //Cambian los valores de retorno, por eso no lo parametrizamos
    if(process_array_len==1) {
        return -1;//No puedo terminar el proceso raiz
    }
    process_array[currentProcess_index].status = TERMINATED;
    print("\n\nPrograma terminado. Pulse ESC para salir",WHITE,process_array[currentProcess_index].position);
    runnable--; //OJO: importa que este aca arriba, si no cambia la logica de runnable de change_context
    change_context();                                           // Tengo que ver cual es el proximo que corro
    return 0;
}

// Teclas especiales


uint8_t suspend_left(){
    if(left_index==-1 || process_array[left_index].status!=RUNNING) return 1;
    process_array[left_index].status = SUSPENDED;
    if(currentProcess_index==left_index){
        //guardo el contexto cuando lo pausaron
        copy_context(getCurrContext(),process_array[left_index].registers);
    }
    //Esto es dependiente de con cual se llama, por eso no se hacen funciones auxiliares
    switch_context(right_index); //Va a intentar correr el de la derecha (y si no se puede correr va con el default)
    return 0;
}

uint8_t suspend_right(){
    if(right_index==-1 || process_array[right_index].status!=RUNNING) return 1;
    process_array[right_index].status = SUSPENDED;
    if(currentProcess_index==right_index){
        copy_context(getCurrContext(),process_array[right_index].registers);
    }
    switch_context(left_index);
    return 0;
}

uint8_t suspend_full(){
    if(full_index==-1 || process_array[full_index].status!=RUNNING) return 1;
    process_array[full_index].status = SUSPENDED;
    if(currentProcess_index == full_index){
        copy_context(getCurrContext(),process_array[full_index].registers);
    }
    switch_context(full_index);
    return 0;
}


static uint8_t restart(uint8_t index){
    if(index==-1 || process_array[index].status!=SUSPENDED) return 1;
    process_array[index].status = RUNNING;
    switch_context(index);
    clear_keyboard_buffer();
    return 1;
}
uint8_t restart_left(){
    return restart(left_index);

}


uint8_t restart_right(){
    return restart(right_index);

}

uint8_t restart_full(){
    return restart(full_index);

}
//TODO: ver de usar para terminate_process
static uint8_t terminate(int8_t index){
    if(process_array_len==1 || index==-1 || process_array[index].status == TERMINATED || process_array[index].status == WAITING) return 1;
    process_array[index].status = TERMINATED;
    runnable--;
    print("Programa terminado. Pulse ESC para salir",WHITE,process_array[index].position);
    change_context();
    return 1;
}
uint8_t kill_left(){
    return terminate(left_index);
}

uint8_t kill_right(){
    return terminate(right_index);
}
uint8_t kill_full(){
    return terminate(full_index);
}

static void switch_context(uint8_t new_index){
    uint64_t* curr_context_arr = getCurrContext();
    if(process_array[currentProcess_index].status == RUNNING && curr_context_arr[RIP]!=(uint64_t)process_array[DEFAULT_PROCESS_INDEX].registers[RIP]){
        //Si esta running y no estaba corriendo en su lugar el proceso defautl (el segundo caso ocurre cuando se reinicio el proceso)
        copy_context(curr_context_arr,process_array[currentProcess_index].registers);
    }
    if(process_array[new_index].status!= RUNNING || new_index==DEFAULT_PROCESS_INDEX){  //Si no esta corriendo, corro a default en su lugar
        //Cuando esta WAITING o TERMINATED, corre en su lugar el default process
        copy_context(process_array[DEFAULT_PROCESS_INDEX].registers,curr_context_arr);//Copio el contexto del default_process
    }else {
        copy_context(process_array[new_index].registers, curr_context_arr);
        currentProcess_index = new_index;
    }

}

void add_two_processes(program_t left, program_t right){
    if(process_array_len==0){
        add_original_process();
    }else{
        copy_context(getCurrContext(),process_array[currentProcess_index].registers);
        runnable--;
    }
    process_array[currentProcess_index].status = WAITING; //Esto hace que switchContext no guarde el contexto => vuelve a empezar si no lo guardo en la mitad con un TT
    left_index = add_process_to_array(left,LEFT);
    right_index = add_process_to_array(right,RIGHT);
    switch_context(right_index); //Cambia el contexto como para que primero se ejecute el programa de la izquierda
}

void add_full_process(program_t process){
    if(process_array_len==0){ //si es el primer proceso, agrego a quien lo ejecuto tambien para poder volver
        add_original_process();
    }else{
        copy_context(getCurrContext(),process_array[currentProcess_index].registers);
        runnable--; //Deja de correr el que lo llama (bash), pues lo cambia a WAITING
    }
    process_array[currentProcess_index].status = WAITING; //Seteo al proceso actual para que espera hasta que termine el nuevo proceso
    full_index = add_process_to_array(process,ALL);
    switch_context(full_index); //Cambia el contexto para que se comience a ejecutar el nuevo programa
}

static void add_original_process(){
    //proces_array_len == ORIGINAL_PROCESS_INDEX
    uint64_t* curr_context = getCurrContext();
    copy_context(curr_context,process_array[process_array_len].registers);
    process_array[process_array_len].position = ALL;
    process_array[process_array_len++].status = WAITING; //Espera a que termine el proceso que lo llamo
    //Voy a agregar el proceso default en un contexto aparte, para evitar problemas
    //Pongo al proceso default en un contexto aparte
    for(int i = 0; i<REGISTERS_COUNT;i++){
        process_array[DEFAULT_PROCESS_INDEX].registers[i] = 0;
    }
    process_array[DEFAULT_PROCESS_INDEX].registers[RSP] = process_array[ORIGINAL_PROCESS_INDEX].registers[RSP] - (5*OFFSET); //Lo dejo arriba de todos
    process_array[DEFAULT_PROCESS_INDEX].registers[RIP] = (uint64_t) &default_process;
    process_array[DEFAULT_PROCESS_INDEX].registers[RFLAGS] = process_array[ORIGINAL_PROCESS_INDEX].registers[RFLAGS];
    process_array[DEFAULT_PROCESS_INDEX].position = ALL;
    process_array[DEFAULT_PROCESS_INDEX].status = RUNNING; //No es importante, no se usa
    //No puedo hacer esto porque cuando hace process_array[0].registers, va a buscar basura
}


//Agrega un proceso al final del arreglo de procesos (en estado RUNNING)
//Devuelve el index del proceso que se agrego
static int add_process_to_array(program_t process, positionType position){
    //Inicializo en contexto del nuevo proceso
    setup_context(process_array[process_array_len].registers,process,process_array[0].registers[RSP]-(process_array_len)*OFFSET,process_array[0].registers[RFLAGS]);
    process_array[process_array_len].position = position;
    process_array[process_array_len++].status = RUNNING;
    runnable++; //aumento el contador de procesos que pueden correr
    return process_array_len-1; //Deuvelve el index del proceso que se agrego
}

//Funciones auxiliares de la implementacion
void setup_context(uint64_t * context, program_t program, uint64_t new_rsp, uint64_t prev_actual_rflags){
    // Inicializa los registros en 0 para el nuevo programa
    for(int  i = 0; i<REGISTERS_COUNT; i++){
        context[i] = 0;
    }
    // Se indica la direccion de memoria donde comienza el proceso (programa)
    context[RIP] = (uint64_t) program.start;
    context[RDI] = program.cant_arg; //Paso la cantidad de argumentos
    context[RSI] = (uint64_t) program.args; //Paso el vector de char*
    // Se indica la direccion de memoria donde comienza el stack local al proceso
    context[RSP] = new_rsp;
    //TODO: cambiar a un valor default con el flag de excepcion prendido
    context[RFLAGS] = prev_actual_rflags; //Esto es importante, si no no vuelve bien
}
void copy_context(uint64_t* source, uint64_t* dest){
    for(int i = 0; i<REGISTERS_COUNT;i++){
        dest[i] = source[i];
    }
}
