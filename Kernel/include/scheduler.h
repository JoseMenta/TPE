

#ifndef TPE_SCHEDULER_H
#define TPE_SCHEDULER_H

#include <stdint.h>

enum statusType {RUNNING = 0, PAUSED, FINISHED};        // Estado en el que se puede encontrar un programa/proceso

enum positionType {ALL = 0, LEFT, RIGHT};               // Posicion del programa en pantalla

typedef struct {                                        // Estructura de un proceso
    uint8_t id;                                         // ID del proceso
    uint64_t rsp;                                       // La direccion del stack
    void * startPoint;                                  // Direccion de memoria inicial del programa
    statusType status;                                  // Estado del proceso
    positionType position;                              // Posicion en pantalla del proceso
} process_t;

uint8_t currentProcess = 0;                             // Proceso corriendo actualmente

void addProcess(void * program);                        // Agrega un proceso al arreglo de procesos
void finishProcess(void);                               // Finaliza un proceso (luego sera borrado)
positionType getCurrentPosition(void);                  // Devuelve la posicion del programa corriendo en el momento

#endif
