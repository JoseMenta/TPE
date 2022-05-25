

#ifndef TPE_QUEUE_H
#define TPE_QUEUE_H

#define MAX 100

typedef struct{
    int elems[MAX];
    int first;
    int last;
    int len;
}queue_t;

void enqueue(queue_t* queue, int elem);
int dequeue(queue_t* queue);
int is_empty(queue_t* queue);

#endif //TPE_QUEUE_H
