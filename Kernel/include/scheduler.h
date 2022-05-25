

#ifndef TPE_SCHEDULER_H
#define TPE_SCHEDULER_H

#include <stdint.h>

typedef enum {RUNNING = 0, PAUSED, FINISHED} statusType;     // Estado en el que se puede encontrar un programa/proceso

typedef enum {ALL = 0, LEFT, RIGHT} positionType;       // Posicion del programa en pantalla

typedef enum {R8 = 0, R9, R10, R11, R12, R13, R14, R15, RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, RIP, RFLAGS} registers; // El orden en el que llegan los registros en el arreglo

#define REGISTERS_COUNT (18)                              // Cantidad de registros

#define OFFSET (1000)                                     // Espacio que se le va a dejar de stack a cada proceso

typedef struct {                                        // Estructura de un proceso
    //uint8_t id;                                       // ID del proceso
    uint64_t registers[REGISTERS_COUNT];                // Estado de los registros cuando se interrumpe el programa
    statusType status;                                  // Estado del proceso
    positionType position;                              // Posicion en pantalla del proceso
} process_t;

void addProcess(void * program);                        // Agrega un proceso al arreglo de procesos
void finishProcess(void);                               // Finaliza un proceso (luego sera borrado)
positionType getCurrentPosition(void);                  // Devuelve la posicion del programa corriendo en el momento

#endif
