#include <stdio.h>

#define BUFFER_SIZE 10

typedef struct _ring_buffer{
    int read_head, write_head;
    int buffer[BUFFER_SIZE];
} ring_buffer_t;


void ring_buffer_create(ring_buffer_t * );
int ring_buffer_enqueue(ring_buffer_t *, int);
int ring_buffer_dequeue(ring_buffer_t * );
int ring_buffer_peek(ring_buffer_t *, int *);



int main(int argc, char* argv[]){
    ring_buffer_t my_ring_buffer;
    ring_buffer_create(&my_ring_buffer);



    printf("hello world ring buffer front = %d", my_ring_buffer.read_head);
    return 0;
}

void ring_buffer_create(ring_buffer_t * ring_buffer){
    ring_buffer->read_head = 0;
    ring_buffer->write_head = 0;
}

int ring_buffer_dequeue(ring_buffer_t * ring_buffer){
    if ((ring_buffer->read_head + 1) > ring_buffer->write_head) return -1;
    if (ring_buffer->read_head < BUFFER_SIZE) {
        ring_buffer->read_head++;
    } else {
        ring_buffer->read_head = 0;
    }
    return 1;
}


int ring_buffer_enqueue(ring_buffer_t * ring_buffer, int item){
    ring_buffer->write_head++;
    if (ring_buffer->write_head == BUFFER_SIZE) ring_buffer->write_head = 0;
    if (ring_buffer->write_head == ring_buffer->read_head) {
        ring_buffer->write_head--;
        return -1;
    }
    ring_buffer->buffer[ring_buffer->write_head];
    return 1;
}