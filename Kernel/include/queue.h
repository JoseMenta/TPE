

#ifndef TPE_QUEUE_H
#define TPE_QUEUE_H

#include <stdint.h>

#define MAX 100
//NOTA: Si no tiene mas espacio en la cola para agregar, no agrega al elemento y se queda con los que ya estaban
//Esto hace que si se escribe mucho y se supera al buffer, algunos caracteres no se guarden y se vuelven a guardar recien
//cuando se sacan algunos leyendo
typedef struct{
    int elems[MAX];
    int first;
    int last;
    int len;
}queue_t;

void enqueue(queue_t* queue, int elem);
uint8_t dequeue(queue_t* queue);
uint8_t is_empty(queue_t* queue);
void clear_queue(queue_t* queue);
#endif //TPE_QUEUE_H
