#include <scheduler.h>
#include <stdint.h>

process_t process_array[5] = {{{0}}};
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
//TODO: setear want_to_return a 0, lo hago asi para debugging
//uint8_t want_to_return = 1; //Flag para ver si el usuario desea volver al proceso anterior al que se esta corriendo luego de que el ultimo haya terminado
//uint8_t left_index = -1;
//uint8_t right_index = -1;
//uint8_t all_index = -1;
//uint8_t runnable = 0;
//void add_full_process(void* process_start);
//static int add_process_to_array(void* process_start, positionType position);
//static void add_original_process();
//static void switch_context(uint8_t new_index);
//Fin implementacion alternativa
positionType get_current_position(){
    return process_array[currentProcess_index].position;
}
//Constantes para poder "suspender" a las semipantallas
//uint8_t left_pid;
//uint8_t right_pid;
uint64_t * getCurrContext(void); //funcion auxiliar, devuelve un vector que fue declarado en interrupts.asm

//Quizas lo mas claro sea hacer las 2 funciones por separado
//-----------------------------------------------------------------------
// add_process: Agrega un proceso que se maneja en toda la pantalla
//-----------------------------------------------------------------------
// Argumentos:
// - start_position: la direccion de memoria donde comienza el proceso
//-----------------------------------------------------------------------
//void add_process_1(void * start_position){
////    uint8_t running_process = 0;
////    for(int i = 0; i < process_array_len; i++){
////        if(process_array[i].status == RUNNING){
////            running_process = i;
////        }
////    }
//    uint64_t * curr_context = getCurrContext();
////    currentProcess_index = process_array_len;
//    setup_context(process_array[process_array_len].registers,start_position,curr_context[RSP]-(process_array_len+1)*OFFSET);
//    process_array[process_array_len].position = ALL;
//    process_array[process_array_len++].status = RUNNING;
//}


//-----------------------------------------------------------------------
// add_left_right_process: Agrega un proceso en left y otro en right
//-----------------------------------------------------------------------
// Argumentos:
// - left_start: la direccion de memoria donde comienza el programa en left
// - left_start: la direccion de memoria donde comienza el programa en right
//-----------------------------------------------------------------------
//void add_left_right_process(void* left_start, void* right_start){
//    //pauso a todos los procesos anteriores
//    for(int i = 0; i < process_array_len; i++){
//        if(process_array[i].status == RUNNING){
//            process_array[i].status = WAITING;
//        }
//    }
//    uint64_t * curr_context = getCurrContext();
//    setup_context(process_array[process_array_len].registers,left_start,curr_context[RSP]-(process_array_len+1)*OFFSET);
//    process_array[process_array_len].position = ALL;
//    process_array[process_array_len++].status = RUNNING;
//
//    // Tiene que cambiar el arreglo curr_context
//    setup_context(process_array[process_array_len].registers,right_start,curr_context[RSP]-(process_array_len+1)*OFFSET);
//    process_array[process_array_len].position = ALL;
//    process_array[process_array_len++].status = RUNNING;
//    //rsp1 == rsp+1000
//    //rsp2 == rsp+2000
//}

//Se puede llamar 2 veces para agregar 2 procesos, total hasta que no vuelve al handler no cambia el contexto
//Con lo que se complica es identificar a cada proceso como left y right si son llamadas separadas, y eso es importante para
//Poder suspender a cada semipantalla (en ese caso, se supone que los procesos se agregan de izquierda a derecha)
//Este es para agregar solo un proceso
void add_process(void * process_start, positionType position){
    uint64_t * curr_context = getCurrContext();
    if(process_array_len == 0){
        //Es el primer proceso que se agrega, me guardo el contexto actual en otro proceso
        //Para poder volver al proceso que lo llamo
        copy_context(curr_context,process_array[process_array_len].registers); //copio el contexto
        process_array[process_array_len].position = ALL;
        process_array[process_array_len++].status = WAITING; //Espera a que termine el proceso que lo llamo
    }
    //guardo el contexto del que agrega el proceso
    copy_context(curr_context,process_array[currentProcess_index].registers);
    //cambio su estado para que se ejecute recien cuando termina el que llamo
    process_array[currentProcess_index].status = WAITING;

    //Se ubica "arriba" del stack del que lo llama
//    setup_context(process_array[process_array_len].registers,process_start,curr_context[RSP]-(process_array_len)*OFFSET);
    //Se ubica Offset veces encima del stack del main_userland
    setup_context(process_array[process_array_len].registers,process_start,process_array[0].registers[RSP]-(process_array_len)*OFFSET);
    process_array[process_array_len].position = position;
    process_array[process_array_len++].status = RUNNING;
    currentProcess_index = process_array_len-1; //Que se corra el que se agrego
    change_context();
//    if(process_array_len == 0){
//        //Es el primer proceso que se agrega, me guardo el contexto actual en otro proceso
//        //Para poder volver al proceso que lo llamo
//        copy_context(curr_context,process_array[process_array_len].registers); //copio el contexto
//        process_array[process_array_len].position = ALL;
//        process_array[process_array_len++].status = WAITING; //Espera a que termine el proceso que lo llamo
//    }
//    //guardo el contexto del que agrega el proceso
//    copy_context(curr_context,process_array[currentProcess_index].registers);
//    //cambio su estado para que se ejecute recien cuando termina el que llamo
//    process_array[currentProcess_index].status = WAITING;
//
//    //Se ubica "arriba" del stack del que lo llama
////    setup_context(process_array[process_array_len].registers,process_start,curr_context[RSP]-(process_array_len)*OFFSET);
//    //Se ubica Offset veces encima del stack del main_userland
//    setup_context(process_array[process_array_len].registers,process_start,process_array[0].registers[RSP]-(process_array_len)*OFFSET);
//    process_array[process_array_len].position = position;
//    process_array[process_array_len++].status = RUNNING;
//    currentProcess_index = process_array_len-1;//Que se corra el ultimo
//    change_context();
}
//TODO: implementar la funcion para llamar a 2 procesos
//exit
uint8_t terminate_process(){
    if(process_array_len==1)
        return-1;//No puedo terminar el proceso raiz
    process_array[currentProcess_index].status = TERMINATED;
    change_context();                                           // Tengo que ver cual es el proximo que corro
    return 0;
}

// Teclas especiales
void suspend_left(){
    for(int i = 0; i<5; i++) {
        if (process_array[2].status == RUNNING && process_array[2].position==LEFT)
            process_array[2].status = SUSPENDED;
            change_context();
    }
}
void suspend_right(){
    for(int i = 0; i<5; i++) {
        if (process_array[2].status == RUNNING && process_array[2].position==RIGHT)
            process_array[2].status = SUSPENDED;
        change_context();
    }
}

//void change_context(){
//    uint64_t * curr_context = getCurrContext();
//    //guardo el contexto del proceso actual
////    if(process_array[currentProcess_index].status==RUNNING) {
//        //Si estaba corriendo el default, no tengo que cambiar los registros
//        copy_context(curr_context, process_array[currentProcess_index].registers);
////    }
//    //Cuento la cantidad de procesos que se pueden correr en el momento
//    int runnable = 0; //Esto lo podria tener calculado de antes
//    for(int i = 0; i<process_array_len;i++){
//        if(process_array[i].status==RUNNING || process_array[i].status==SUSPENDED){
//            runnable++;
//        }
//    }
//    int last_waiting = process_array_len-1;
//    for(;last_waiting>0 && process_array[last_waiting].status!=WAITING;last_waiting--); //voy hasta el padre que esta mas abajo
//    //Si ya termminaron los que estaban corriendo, tengo que ir al padre
//    if(runnable==0){//Terminaron los subprocesos, tengo que volver al que los llama
////        int i = process_array_len-1;
////        for(; i>=0 && process_array[i].status!=WAITING; i--);//deberia cortar siempre por la segunda condicion
////            if(process_array[i].status==WAITING) break;
////            me quedo con el padre que esta "mas abajo" en el arbol
////            puedo pensar al arreglo como una impresion por niveles de un arbol
//        currentProcess_index = last_waiting;
//        copy_context(process_array[currentProcess_index].registers,curr_context);//gurado el contexto
//        process_array[currentProcess_index].status = RUNNING;
//        //"Elimino" a todos los procesos terminados
//        process_array_len = currentProcess_index+1;
//    }else{
//        if(last_waiting+1!=process_array_len-1){//Si hay 2 procesos que se pueden correr
//            //Elijo el que no se este corriendo de los que estaban
//            currentProcess_index = (currentProcess_index==last_waiting+1)?last_waiting+2:last_waiting+1;
//            //Esta mal lo de arriba, el otro puede estar terminado
//        }//Si no, tengo que volver a correr el mismo
//        if(process_array[currentProcess_index].status==SUSPENDED){
//            curr_context[RIP] = (uint64_t )default_process;
//        }else{
//            copy_context(process_array[currentProcess_index].registers,curr_context);
//        }
//    }
//
//}

void change_context(){
    if(process_array_len==0) return; //Para que no cambie el contexto si no hay procesos (el el TT del kernel)
    //runnable: guarda la cantidad de procesos que estan el estado RUNNING o SUSPENDED
    //TODO: que ya este calculado
    uint64_t* curr_context = getCurrContext();
//    if(process_array[currentProcess_index].status==RUNNING){
//        // Guardo el contexto que llega del proceso donde ocurre la interrupcion
//        // No lo guardo si esta suspendido, pues si no estaria cambiando el RIP
//        copy_context(curr_context,process_array[currentProcess_index].registers)
//    }
    int runnable = 0;
    for(int i = 0; i<process_array_len; i++){
        if(process_array[i].status==RUNNING || process_array[i].status==SUSPENDED)
            runnable++;
    }
    if(runnable > 2){
        // Lanzar excepcion
        // Estamos haciendo algo mal
    }
    else if(runnable == 2){
        //TODO: verificar que la primera condicion siempre es verdadera
        if(process_array_len >= 4 && process_array[2+LEFT].status == SUSPENDED && process_array[2+RIGHT].status == SUSPENDED){
            // Ambos procesos estan parados
            // Al que esta corriendo, lo reemplazo otra vez por el default_process
            curr_context[RIP] = (uint64_t) default_process;
            return;
        }
        // Si no entre en el if anterior, algunos de los procesos se esta ejecutando
        else if(currentProcess_index == 2 + LEFT) {           // Esta corriendo el proceso de la izquierda
            if (process_array[2 + RIGHT].status != RUNNING) {
                // No hace nada
                // sigue con left
                return;
            } else {
                // Cambiar contexto: Esta corriendo el de la izquierda y quiero cambiar al de la derehca

                // Guardo el contexto del proceso de la izquierda
                copy_context(curr_context, process_array[2+LEFT].registers);
                // Cambio el contexto al proceso de la derecha
                copy_context(process_array[2+RIGHT].registers, curr_context);
                // Actualizo el currentProcess_index al proceso derecho
                currentProcess_index = 2 + RIGHT;
            }
        }
        else {                                                       // Esta corriendo el proceso de la derecha
            if (process_array[2 + LEFT].status != RUNNING) {
                // No hace nada: sigue con el proceso de la derecha
                return;
            } else {
                // Cambiar contexto: Esta corriendo el de la derecha y quiero cambiar al de la izquierda

                // Guardo el contexto del proceso de la derecha
                copy_context(curr_context, process_array[2+RIGHT].registers);
                // Cambio el contexto al proceso de la izquierda
                copy_context(process_array[2+LEFT].registers, curr_context);
                // Actualizo el currentProcess_index al proceso izquierdo
                currentProcess_index = 2 + LEFT;
            }
        }
    }
    else if(runnable == 1){
//        //Aca ya suponemos que curr_process_index esta RUNNING o SUSPENDED
//        //En el primero, entra con curr_process_index==0;
//        if(process_array[currentProcess_index].status == WAITING){
//            //Tengo que correr el que se puede correr, y dejar a este
//
//        }
        if(process_array[currentProcess_index].status == SUSPENDED){
            // Ciclo nop
            curr_context[RIP] = (uint64_t) default_process;
            return;
        }
        else if(process_array[currentProcess_index].status == RUNNING){
            //Lo guardamos para el caso donde se acaba de insertar
            copy_context(process_array[currentProcess_index].registers,curr_context);
            // Sigue en el proceso actual
            return;
        }
    }
    else if(runnable == 0){
        // Si no hay procesos corriendo, va a rejecutar el ultimo proceso que se detuvo (el proceso mas joven
        // , con indice más grande en el arreglo, en waiting
        int i = process_array_len-1;
        for(; process_array[i].status != WAITING; i--);
        currentProcess_index = i;
        process_array[currentProcess_index].status = RUNNING;
        process_array_len = currentProcess_index+1;
        copy_context(process_array[currentProcess_index].registers,curr_context);
    }
    return;
}

void copy_context(uint64_t* source, uint64_t* dest){
    for(int i = 0; i<REGISTERS_COUNT;i++){
        dest[i] = source[i];
    }
}
//-----------------------------------------------------------------------
// setup_context: Inicializa los registros para el nuevo proceso
//-----------------------------------------------------------------------
// Argumentos:
// - context: el arreglo donde se almacenan los registros del proceso
// - new_rip: la direccion de memoria donde comienza el proceso
// - new_rsp: la direccion de memoria donde comienza el stack para el proceso
//-----------------------------------------------------------------------
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


//-----------------------------------------------------------------------
// change_context: Cambia los registros cuando cambio los programas
//-----------------------------------------------------------------------
// Argumentos:
//-----------------------------------------------------------------------
//void change_context(){
//    //Si no hay procesos corriendo (en el primer llamado):
//    //  solo pasar el nuevo contexto que esta en la estructura en los registros
//    //  poner el estado en RUNNING
//    //  return
//    //Si hay procesos corriendo:
//    //  Guardar el contexto actual (que saco con el getter de asm) en la estructura
//    //  No cambiamos el estado del proceso que sacamos, pues sigue corriendo
//    //  ver cual es el siguiente proceso a ejecutar (que es el siguiente en estado running)
//    //  Cargar el contexto del siguiente proceso
//    //  return
//    return;
//}
////Lo siguiente es una implementacion alternativa de la logica del scheduler
//void add_process(void * process_start, positionType position){
//    add_full_process(process_start);
//}
//void change_context(){
//    if(process_array_len==0) return; //No hay procesos disponibles para cambiar
//    uint64_t* curr_context_arr = getCurrContext();
//    if(runnable == 2){
//        if(process_array[left_index].status==SUSPENDED && process_array[right_index].status==SUSPENDED){\
//        //Si ambos estan suspendidos, corro el proceso default
//            curr_context_arr[RIP] = (uint64_t) &default_process;
//            return;
//        }
//        if(currentProcess_index==left_index){
//            //Si esta corriendo el proceso de la izquierda
//            if(process_array[right_index].status!=RUNNING){
//                //Si el proceso de la derecha termino o esta suspendido
//                return; //Me quedo en el proceso de la izuierda
//            }else{
//                //Cambio para que siga el proceso de la derecha
//                switch_context(right_index);
//                return;
//            }
//        }else{
//            // Esta corriendo el proceso de la derecha
//            if (process_array[left_index].status!=RUNNING) {
//                // No hace nada: sigue con el proceso de la derecha
//                return;
//            } else {
//                //Cambio para que siga el proceso de la izquierda
//                switch_context(left_index);
//            }
//        }
//    }else if (runnable==1){
//        //TODO: sacar este caso
//        //Tenbo que seguir como estaba antes, sin cambios
//        return;
//    }else if(runnable==0){
//        //Terminaron todos los procesos que se podian correr
//        if(!want_to_return){
//            curr_context_arr[RIP] = (uint64_t) &default_process; //Me quedo esperando a que quiera volver (voy a estar en el contexto de uno de los que estaba corriendo)
//        }
//        int i = process_array_len-1;
//        for(; process_array[i].status != WAITING; i--); //Busco el index del ultimo proceso WAITING
//        switch_context(i);//Cambio el contexto al de ese proceso
//        process_array_len = i +1;//Elimino a los procesos que venian despues de el
//        return;
//    }
//    return;
//}
//
//uint8_t terminate_process(){
//    if(process_array_len==1) {
//        return -1;//No puedo terminar el proceso raiz
//    }
//    process_array[currentProcess_index].status = TERMINATED;
//    runnable--; //OJO: importa que este aca arriba, si no cambia la logica de runnable de change_context
//    change_context();                                           // Tengo que ver cual es el proximo que corro
//    return 0;
//}
//
//// Teclas especiales
//void suspend_left(){
//    if(left_index==-1) return;
//    process_array[left_index].status = SUSPENDED;
//    change_context();
//}
//void suspend_right(){
//    if(right_index==-1) return;
//    process_array[right_index].status = SUSPENDED;
//    change_context();
//}
//
//void add_two_processes(void* left_start, void* right_start){
//    if(process_array_len==0){
//        add_original_process();
//    }
//    process_array[currentProcess_index].status = WAITING;
//    int left_index = add_process_to_array(left_start,LEFT);
//    int right_index = add_process_to_array(right_start,RIGHT);
//    switch_context(left_index); //Cambia el contexto como para que primero se ejecute el programa de la izquierda
//}
//void add_full_process(void* process_start){
//    if(process_array_len==0){ //si es el primer proceso, agrego a quien lo ejecuto tambien para poder volver
//        add_original_process();
//    }
//    process_array[currentProcess_index].status = WAITING; //Seteo al proceso actual para que espera hasta que termine el nuevo proceso
//    int new_index = add_process_to_array(process_start,ALL);
//    switch_context(new_index); //Cambia el contexto para que se comience a ejecutar el nuevo programa
//}
////Agrega el proceso original, que es el primer proceso que llama a sys_exec
//static void add_original_process(){
//    uint64_t* curr_context = getCurrContext();
//    copy_context(curr_context,process_array[process_array_len].registers);
//    process_array[process_array_len].position = ALL;
//    process_array[process_array_len++].status = WAITING; //Espera a que termine el proceso que lo llamo
//    //No puedo hacer esto porque cuando hace process_array[0].registers, va a buscar basura
//}
////Agrega un proceso al final del arreglo de procesos (en estado RUNNING)
////Devuelve el index del proceso que se agrego
//static int add_process_to_array(void* process_start, positionType position){
//    //Inicializo en contexto del nuevo proceso
//    setup_context(process_array[process_array_len].registers,process_start,process_array[0].registers[RSP]-(process_array_len)*OFFSET);
//    process_array[process_array_len].position = position;
//    process_array[process_array_len++].status = RUNNING;
//    runnable++; //aumento el contador de procesos que pueden correr
//    return process_array_len-1;
//}
////Cambia el contexto para que pase del proceso en currentProcess_index al proceso en el index new_index
//static void switch_context(uint8_t new_index){
//    uint64_t* curr_context_arr = getCurrContext();
//    if(process_array[currentProcess_index].status!=WAITING){
//        //Solo guardo el contexto si no estaba en WAITING, si no no lo guardo para no cambiar los registros (en especial los flags)
//        copy_context(curr_context_arr,process_array[currentProcess_index].registers); //Guardo el contexto actual en el proceso que se estaba corriendo
//    }
//    copy_context(process_array[new_index].registers,curr_context_arr);//Cargo el contexto del nuevo proceso para que se configure antes de iretq
//    if(process_array[new_index].status==SUSPENDED){
//        //Si esta suspendido, cambio el RIP para que en su lugar se ejecute el default_process
//        curr_context_arr[RIP] = (uint64_t) &default_process;
//    }
//    currentProcess_index = new_index; //Indico que se comienza a ejecutar ese proceso
//}
////Funciones auxiliares de la implementacion
//void setup_context(uint64_t * context, void * new_rip, uint64_t new_rsp){
//    // Inicializa los registros en 0 para el nuevo programa
//    for(int  i = 0; i<REGISTERS_COUNT; i++){
//        context[i] = 0;
//    }
//    // Se indica la direccion de memoria donde comienza el proceso (programa)
//    context[RIP] = (uint64_t) new_rip;
//    // Se indica la direccion de memoria donde comienza el stack local al proceso
//    context[RSP] = new_rsp;
//}
//void copy_context(uint64_t* source, uint64_t* dest){
//    for(int i = 0; i<REGISTERS_COUNT;i++){
//        dest[i] = source[i];
//    }
//}