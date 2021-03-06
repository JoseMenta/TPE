#include <scheduler.h>
#include <interrupts.h>
#include <stdint.h>
#include <keyboard.h>
#include <video_driver.h>

#define ORIGINAL_PROCESS_INDEX 0
#define DEFAULT_PROCESS_INDEX 4
//Arreglo utilizado para almacenar el contexto de los procesos
//En esta implementacion, acepta como maximo 5 procesos
static process_t process_array[5] = {{{0}}};
static uint8_t process_array_len = 0;
static uint8_t currentProcess_index = 0; //Arranca en 0, el proceso default



//El arreglo va a estar compuesto por
//[main_userland,bash,...(procesos que llama el bash), default_process]
//Si la cantidad de procesos es 4 => se estan corriendo pantallas divididas
//Si es 3 => se esta corriendo solo un proceso encima de la terminal
//Si es 2 => se esta corriendo solo la terminal
//Si es 1 => Esta en userland
//Dejamos main_userland para poder terminar con el kernel sin tener que forzar la salida
static void setup_context(uint64_t* context,program_t program, uint64_t new_rsp);
static void copy_context(uint64_t* source, uint64_t* dest);
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
int process_array_is_empty(){
    return process_array_len==0;
}
positionType get_current_position(){
    return process_array[currentProcess_index].position;
}

//----------------------------------------------------------------------
// change_context: cambia el contexto cuando llega una interrupcion del timer tick
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Es una funcion que se llama desde el handler del timer tick y cumple con lo siguiente
// Si no hay procesos cargados: no cambia el contexto
// Si hay un unico proceso cargado para correr:
//      -si el proceso esta corriendo(RUNNING): lo deja corriendo
//      -si el proceso esta pausado(SUSPENDED): cambia el contexto para correr el default process (hasta que termine la pausa)
//      -si el proceso esta terminado(TERMINATED): espera a que el usuario indique con la tecla ESC que quiere volver al proceso que lo llamo
//          (mientras corre el default process)
// Si hay 2 procesos cargados para correr:
//      -si los 2 procesos estan corriendo(RUNNING): va alternando entre los contextos
//      -si uno esta pausado o terminado y el otro esta corriendo: deja o cambia el contexto para seguir con el que esta corriendo
//      -si ambos estan pausados: corre el proceso default
//      -si ambos estan terminados: corre el proceso default hasta que el usuario indique con la tecla ESC que desea volver al proceso que lo llamo
// En cualquier estado, si el usuario presiona la tecla ESC, se vuelve al proceso que llamo al que se estaba corriendo (en el caso del bash, vuelve
// al main en sampleCodeModule para volver al kernel y terminar con el mismo)
//----------------------------------------------------------------------
// La funcion cambia o mantiene un vector definido en interrupts.asm, que tiene el contexto del proceso en el que ocurrio la interrupcion
// y lo cambia o no para que tenga el contexto del proceso que se debe ejecutar cuando termine la interrupcion
//----------------------------------------------------------------------
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
//----------------------------------------------------------------------
// terminate_process: termina con el proceso que se estaba corriendo
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Si no hay procesos cargados, no cambia el estado
// Si se llama desde un proceso que se cargo, lo marca como terminado y llama a change_context
// Tambien imprime un mensaje en la pantalla del proceso que lo llamo para indicarle al usuario como seguir
//----------------------------------------------------------------------
// Retorno
//  -1 si no habia procesos para terminar
//  0 si termino el proceso
//----------------------------------------------------------------------
uint8_t terminate_process(){
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

//----------------------------------------------------------------------
// suspend_left: funcion que se usa para suspender al proceso de la izuierda con una combinacion de teclas
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Esta funcion se llama desde keyboard.c cuando el usuario presiona la combinacion de teclas especificada para
// pausar el proceso de la izquierda
// Si no hay un proceso corriendo en la pantalla izquierda, no cambia el estado
//----------------------------------------------------------------------
// Retorna: 0 si logro suspender al proceso, 1 si no
//----------------------------------------------------------------------
uint8_t suspend_left(){
    if(left_index==-1 || process_array[left_index].status!=RUNNING) return 1;
    process_array[left_index].status = SUSPENDED;
    if(currentProcess_index==left_index){
        //guardo el contexto cuando lo pausaron
        copy_context(getCurrContext(),process_array[left_index].registers);
    }
    switch_context(right_index); //Va a intentar correr el de la derecha (y si no se puede correr va con el default)
    return 0;
}
//----------------------------------------------------------------------
// suspend_right: funcion que se usa para suspender al proceso de la derecha con una combinacion de teclas
//----------------------------------------------------------------------
// Esta funcion se llama desde keyboard.c cuando el usuario presiona la combinacion de teclas especificada para
// pausar el proceso de la derecha
// Si no hay un proceso corriendo en la pantalla derecha, no cambia el estado
//----------------------------------------------------------------------
// Retorna: 0 si logro suspender al proceso, 1 si no
//----------------------------------------------------------------------
uint8_t suspend_right(){
    if(right_index==-1 || process_array[right_index].status!=RUNNING) return 1;
    process_array[right_index].status = SUSPENDED;
    if(currentProcess_index==right_index){
        copy_context(getCurrContext(),process_array[right_index].registers);
    }
    switch_context(left_index);
    return 0;
}
//----------------------------------------------------------------------
// suspend_left: funcion que se usa para suspender al proceso de la pantalla completa con una combinacion de teclas
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Esta funcion se llama desde keyboard.c cuando el usuario presiona la combinacion de teclas especificada para
// pausar el proceso de la pantalla completa
// Si no hay un proceso corriendo en la pantalla completa, no cambia el estado
//----------------------------------------------------------------------
// Retorna: 0 si logro suspender al proceso, 1 si no
//----------------------------------------------------------------------
uint8_t suspend_full(){
    if(full_index==-1 || process_array[full_index].status!=RUNNING) return 1;
    process_array[full_index].status = SUSPENDED;
    if(currentProcess_index == full_index){
        copy_context(getCurrContext(),process_array[full_index].registers);
    }
    switch_context(full_index);
    return 0;
}
//----------------------------------------------------------------------
// restart: reinicia el proceso en el index que se pasa por parametro
//----------------------------------------------------------------------
// Argumentos:
//  index: el index del proceso que se desea reanudar
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Es una funcion auxiliar que se encarga de cambiar el estado de un proceso
// desde SUSPENDED a RUNNING, cambiando el contexto al proceso indicado inmediatamente
// Limpia el buffer de teclado para que el proceso no sea afectado por las teclas presionadas
// mientras estaba pausado
// Si el proceso indicado no es correcto, no cambia el contexto
//----------------------------------------------------------------------

static uint8_t restart(uint8_t index){
    if(index==-1 || process_array[index].status!=SUSPENDED) return 1;
    process_array[index].status = RUNNING;
    switch_context(index);
    clear_keyboard_buffer();
    return 1;
}
//----------------------------------------------------------------------
// restart_left: reinicia el proceso de la izquierda (pasa de SUSPENDED a RUNNING)
//----------------------------------------------------------------------
uint8_t restart_left(){
    return restart(left_index);

}
//----------------------------------------------------------------------
// restart_right: reinicia el proceso de la derecha (pasa de SUSPENDED a RUNNING)
//----------------------------------------------------------------------
uint8_t restart_right(){
    return restart(right_index);

}
//----------------------------------------------------------------------
// restart_full: reinicia el proceso de la pantalla completa (pasa de SUSPENDED a RUNNING)
//----------------------------------------------------------------------
uint8_t restart_full(){
    return restart(full_index);

}
//----------------------------------------------------------------------
// terminate: termina con un proceso en el index que se pasa como parametro
//----------------------------------------------------------------------
// Argumentos:
//  index: el index del proceso que se desea terminar
//----------------------------------------------------------------------
// Es una funcion auxiliar que termina el proceso que se pasa (lo lleva al estado TERMINATED)
// Si el proceso indicado no es correcto o el mismo no esta corriendo, no cambia el contexto.
// Si no, imprime un mensaje indicando al usuario como volver al proceso anterior, y llama a change_context
//----------------------------------------------------------------------
static uint8_t terminate(int8_t index){
    if(process_array_len==1 || index==-1 || process_array[index].status == TERMINATED || process_array[index].status == WAITING) return 1;
    process_array[index].status = TERMINATED;
    runnable--;
    print("Programa terminado. Pulse ESC para salir",WHITE,process_array[index].position);
    change_context();
    return 1;
}
//----------------------------------------------------------------------
// kill_left: termina con el proceso de la izquierda
//----------------------------------------------------------------------
uint8_t kill_left(){
    return terminate(left_index);
}
//----------------------------------------------------------------------
// kill_right: termina con el proceso de la derecha
//----------------------------------------------------------------------
uint8_t kill_right(){
    return terminate(right_index);
}
//----------------------------------------------------------------------
// kill_full: termina con el proceso de la pantalla completa
//----------------------------------------------------------------------
uint8_t kill_full(){
    return terminate(full_index);
}
//----------------------------------------------------------------------
// switch_context: cambia el contexto al proceso indicado por new_index
//----------------------------------------------------------------------
// Argumentos:
//  new_index: el index del proceso al que se desea pasar
//----------------------------------------------------------------------
// Se encarga de guardar el contexto del proceso que se estaba corriendo (si en lugar
// de este no se estaba corriendo el proceso default) y cambia el estado al del proceso
// indicado por new_index si su estado es RUNNING o al del defautl_process en otro caso
//----------------------------------------------------------------------
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
//----------------------------------------------------------------------
// add_two_processes: agrega 2 procesos, uno a la izquierda y otro a la derecha
//----------------------------------------------------------------------
// Argumentos:
//  left: el programa que va a ir a la izquierda
//  right: el programa que va a ir a la derecha
//----------------------------------------------------------------------
// En el caso donde son los primeros procesos que se agregan, se guarda el contexto del proceso que los agrega
// En otro caso, se marca al proceso que estaba corriendo como WAITING y se guarda su estado para poder volver a
// el mismo cuando terminan los que se agregan
// Corre primero el proceso de la derecha
//----------------------------------------------------------------------
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
//----------------------------------------------------------------------
// add_full_process: agrega el proceso indicado
//----------------------------------------------------------------------
// Argumentos:
//  process: el proceso que se desea agregar en la pantalla completa
//----------------------------------------------------------------------
// En el caso donde es el primer proceso que se agrega, guarda el contexto del proceso que llama a la funcion
// En otro caso, se marca al proceso que estaba corriendo como WAITING y se guarda su estado para poder volver
// a es cuando termina el que se inserta
// Corre inmediatamente al proceso indicado
//----------------------------------------------------------------------
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
//----------------------------------------------------------------------
// add_original_process: funcion auxiliar que agrega al contexto actual como un proceso
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Es una funcion que se utiliza en las 2 anteriores para cargar al contexto del que llama a la funcion
// Ademas, agrega al vector de procesos al default_process, necesario para poder pausar a lo que se agregue
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// add_process_to_array: agrega un proceso al final del arreglo de procesos
//----------------------------------------------------------------------
// Argumentos:
//  process: el programa que se desea agregar al arreglo
//  position: posicion de la pantalla del programa
//----------------------------------------------------------------------
// Agrega el proceso que se pasa con la estructura program_t (que la inicializa el usuario en UserSpace)
// en la posicion indicada (ALL, LEFT, RIGHT)
//----------------------------------------------------------------------
// Retorno: el index del proceso que se agrego en el arreglo de procesos
//----------------------------------------------------------------------
static int add_process_to_array(program_t process, positionType position){
    //Inicializo en contexto del nuevo proceso
    setup_context(process_array[process_array_len].registers,process,process_array[0].registers[RSP]-(process_array_len)*OFFSET);
    process_array[process_array_len].position = position;
    process_array[process_array_len++].status = RUNNING;
    runnable++; //aumento el contador de procesos que pueden correr
    return process_array_len-1; //Deuvelve el index del proceso que se agrego
}
//----------------------------------------------------------------------
// setup_context: inicializa el contexto de un programa
//----------------------------------------------------------------------
// Argumentos:
//  context: puntero del arreglo de uint64_t que va a tener el contexto del programa que se desea setear
//  program: informacion del programa que se desea setear
//  new_rsp: valor que debe tomar rsp en el programa
//----------------------------------------------------------------------
// La funcion recibe un arreglo de uint64_t, que es el arreglo de los registros del programa
// que se busca inicializar.
// Inicializa a RIP con la direccion de la primera instruccion del programa, RDI con la cantidad de argumentos,
// RSI con un char**, que son los strings con los argumentos (para que el programa pueda recibir a los ultimos 2 como
// argumentos, y a RSP con un valor new_rsp, que se calcula como el RSP del proceso original - index_proceso_nuevo * offset,
// donde offset es un valor definido. Esto es para separar a los stacks de los procesos en memoria.
//----------------------------------------------------------------------
static void setup_context(uint64_t * context, program_t program, uint64_t new_rsp){
    // Inicializa los registros en 0 para el nuevo programa
    for(int  i = 0; i<REGISTERS_COUNT; i++){
        context[i] = 0;
    }
    // Se indica la direccion de memoria donde comienza el proceso (programa)
    context[RIP] = (uint64_t) program.start;
    context[RDI] = program.cant_arg; //Paso la cantidad de argumentos
    context[RSI] = (uint64_t) program.args; //Paso el vector de char**
    context[RSP] = new_rsp;
    context[RFLAGS] = 0x200; //Dejo prendido el flag de Interrupt enable flag, necesario para poder volver de la interrupcion
}
//----------------------------------------------------------------------
// copy_context: funcion auxiliar para copiar los contextos
//----------------------------------------------------------------------
// Argumentos:
//  source: el arreglo del cual saca los valores
//  dest: el arreglo donde copia los valores
//----------------------------------------------------------------------
// Funcion que copia los contenidos de un vector de uint64_t a otro, utilizada para copiar los valores
// de los arreglos de registros
//----------------------------------------------------------------------
static void copy_context(uint64_t* source, uint64_t* dest){
    for(int i = 0; i<REGISTERS_COUNT;i++){
        dest[i] = source[i];
    }
}
