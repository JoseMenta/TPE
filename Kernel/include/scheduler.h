

#ifndef TPE_SCHEDULER_H
#define TPE_SCHEDULER_H

#include <stdint.h>
//RUNNING: El proceso debe correrse
//WAITING: El proceso esta esperando a que terminen procesos que el ejecuto
//SUSPENDED: El proceso fue suspendido (con una de las teclas indicadas). En su lugar se ejecuta el proceso default (while 1)
//FINISHED: El proceso fue terminado, ya se puede eliminar cuando se reinicie el proceso que lo ejecuto
typedef enum {RUNNING = 0, WAITING, SUSPENDED, TERMINATED} statusType;     // Estado en el que se puede encontrar un programa/proceso

typedef enum {ALL = 0, LEFT, RIGHT} positionType;       // Posicion del programa en pantalla

typedef enum {R8 = 0, R9, R10, R11, R12, R13, R14, R15, RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, RIP, RFLAGS} registers; // El orden en el que llegan los registros en el arreglo

#define REGISTERS_COUNT (18)                              // Cantidad de registros

#define OFFSET (10000)                                     // Espacio que se le va a dejar de stack a cada proceso

typedef struct {                                        // Estructura de un proceso
    uint64_t registers[REGISTERS_COUNT];                // Estado de los registros cuando se interrumpe el programa
    statusType status;                                  // Estado del proceso
    positionType position;                              // Posicion en pantalla del proceso
} process_t;

void add_process(void * process_start, positionType position);                       // Agrega un proceso al arreglo de procesos
uint8_t terminate_process(void);                               // Finaliza un proceso (luego sera borrado)
positionType get_current_position(void);                  // Devuelve la posicion del programa corriendo en el momento
void suspend_left();
void suspend_right();
void change_context();
#endif
