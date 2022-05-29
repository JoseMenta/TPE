

#ifndef TPE_SCHEDULER_H
#define TPE_SCHEDULER_H

#include <stdint.h>

// Estado en el que se puede encontrar un programa/proceso
// RUNNING: El proceso debe correrse
// WAITING: El proceso esta esperando a que terminen procesos que el ejecuto
// SUSPENDED: El proceso fue suspendido (con una de las teclas indicadas). En su lugar se ejecuta el proceso default (while 1)
// FINISHED: El proceso fue terminado, ya se puede eliminar cuando se reinicie el proceso que lo ejecuto
typedef enum {RUNNING = 0, WAITING, SUSPENDED, TERMINATED} statusType;

typedef enum {ALL = 0, LEFT, RIGHT} positionType;       // Posicion del programa en pantalla
//ACTUAL_RFLAGS: Los flags cuando se prdoucen la interrupcion (cuando se inicia el proceo, se deja como estaban en el proceso que los ejecuto)
//ACTUAL_RSP: El rsp cuando se produce la excepcion (no se usa en el scheduler, es para las excepciones)
typedef enum {R8 = 0, R9, R10, R11, R12, R13, R14, R15, RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, RIP, RFLAGS, ACTUAL_RFLAGS,ACTUAL_RSP} registers; // El orden en el que llegan los registros en el arreglo

#define REGISTERS_COUNT (19)                              // Cantidad de registros (NOTA: no incluye a ACTUAL_RSP)

#define OFFSET (1000)                                     // Espacio que se le va a dejar de stack a cada proceso

typedef struct {                                        // Estructura de un proceso
    uint64_t registers[REGISTERS_COUNT];                // Estado de los registros cuando se interrumpe el programa
    statusType status;                                  // Estado del proceso
    positionType position;                              // Posicion en pantalla del proceso
} process_t;

void add_process(void * process_start, positionType position);      // Agrega un proceso al arreglo de procesos
uint8_t terminate_process(void);                                    // Finaliza un proceso (luego sera borrado)
positionType get_current_position(void);                            // Devuelve la posicion del programa corriendo en el momento

uint8_t suspend_left();                                                 // Funciones para la logica de pausar y reanudar un proceso
uint8_t suspend_right();
uint8_t suspend_full();
uint8_t restart_left();
uint8_t restart_right();
uint8_t restart_full();

void change_context();                                              // Cambia de proceso si es que puede
void add_full_process(void* process_start);                         // Agrega un nuevo proceso al arreglo de procesos
void add_two_processes(void* left_start, void* right_start);        // Agrega dos nuevos procesos (left y right) al arreglo de procesos
#endif
