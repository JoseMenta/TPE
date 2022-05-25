#include <queue.h>

void enqueue(queue_t* queue, int elem){
    if(queue->len==MAX) return;//No lo puede agregar si no hay espacio
    queue->elems[queue->last] = elem;
    queue->last = (queue->last+1)%MAX;
    (queue->len)++;
}

int is_empty(queue_t* queue){
   return queue->len==0;
}

int dequeue(queue_t* queue){
    if(queue->len==0) return '\0'; //si no hay caracteres para leer, guarda 0
    int ans = queue->elems[queue->first];
    (queue->len)--;
    queue->first = (queue->first + 1)%MAX;
    return ans;
}
