#include <scheduler.h>
#include <stdint.h>

process_t process_array[5] = {{{0}}};
uint8_t process_array_len = 0;
uint8_t currentProcess_index = 0; //Arranca en 0, el proceso default

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

positionType get_current_position(){
    return process_array[currentProcess_index].position;
}
//Constantes para poder "suspender" a las semipantallas
//uint8_t left_pid;
//uint8_t right_pid;
uint64_t * getCurrContext(void); //funcion auxiliar
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
    change_context();
}

//exit
uint8_t terminate_process(){
    if(process_array_len==1) return-1;//No puedo terminar el proceso raiz
    process_array[currentProcess_index].status = TERMINATED;
    change_context();//tengo que ver cual es el proximo que corro
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
    if(runnable == 2){
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
    if(runnable == 1){
        if(process_array[currentProcess_index].status == SUSPENDED){
            // Ciclo nop
            curr_context[RIP] = (uint64_t) default_process;
            return;
        }
        else if(process_array[currentProcess_index].status == RUNNING){
            // Sigue en el proceso actual
            return;
        }
    }
    if(runnable == 0){
        int i= process_array_len-1;
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


