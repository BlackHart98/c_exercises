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

    int my_data[] = {0,3,4,8,2,1,7};
    ring_buffer_enqueue(&my_ring_buffer, my_data[0]);

    int val;
    printf("hello world ring buffer front = %d\n", my_ring_buffer.read_head);

    ring_buffer_dequeue(&my_ring_buffer);

    if (ring_buffer_peek(&my_ring_buffer, &val) == -1) {
        printf("Empty ahh\n");
    } else {
        printf("hello world, %d\n", val);
    }

    return 0;
}


void ring_buffer_create(ring_buffer_t * ring_buffer){
    ring_buffer->read_head = 0;
    ring_buffer->write_head = 0;
}


int ring_buffer_dequeue(ring_buffer_t * ring_buffer){
    if (ring_buffer->read_head == ring_buffer->write_head) return -1;
    ring_buffer->read_head = ((ring_buffer->read_head + 1) % BUFFER_SIZE);
    return 1;
}


int ring_buffer_enqueue(ring_buffer_t * ring_buffer, int item){
    int next_idx = ((ring_buffer->write_head + 1) % BUFFER_SIZE);
    if (next_idx == ring_buffer->read_head) return -1;
    ring_buffer->buffer[ring_buffer->write_head] = item;
    ring_buffer->write_head = next_idx;
    return 1;
}


int ring_buffer_peek(ring_buffer_t * ring_buffer, int * item){
    if (ring_buffer->write_head == ring_buffer->read_head) return -1;
    *item = ring_buffer->buffer[ring_buffer->read_head];
    return 1;
}