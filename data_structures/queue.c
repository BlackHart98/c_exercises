#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 10

typedef struct _queue {
    int rear;
    int front;
    int content[MAX_THREADS];
} queue_t;

void queue_create(queue_t *);
void enqueue(queue_t *, int);
void dequeue(queue_t *);
int peek(queue_t *, int*);

int main(int argc, char* argv[]){
    queue_t my_queue;
    queue_create(&my_queue);

    enqueue(&my_queue, (int)3);
    int val;
    peek(&my_queue, &val);
    printf("here is the value = %d\n", val);
    if (peek(&my_queue, &val) == -1) printf("queue is empty\n");

    dequeue(&my_queue);

    if (peek(&my_queue, &val) == -1) printf("queue is empty\n");

    return 0;
}

void queue_create(queue_t *queue){
    queue->rear = 0;
    queue->front = 0;
}

void enqueue(queue_t *queue, int item){
    queue->content[queue->rear] = item;
    queue->rear++;
}


void dequeue(queue_t *queue){
    if (queue->rear == queue->front) return;
    // shift right
    for (int i = 0; i < queue->rear; i++){
        queue->content[i] = queue->content[i + 1];
    }
    queue->rear--;
}


int peek(queue_t * queue, int* item){
    if (queue->rear == queue->front) return -1;
    *item = queue->content[queue->front];
    return 1;
}