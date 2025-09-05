#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#define BUFFER_SIZE 3
#define len(T, obj) (sizeof(obj)/sizeof(T))

// lock-based ring buffer
typedef struct _ring_buffer_t{
    int read_head, write_head;
    int buffer[BUFFER_SIZE];
    pthread_mutex_t ring_buffer_mutex;
    pthread_cond_t is_empty;
    pthread_cond_t is_full;
} ring_buffer_t;


void ring_buffer_create(ring_buffer_t *);
int ring_buffer_empty(const ring_buffer_t *);
int ring_buffer_full(const ring_buffer_t *);
void enqueue(ring_buffer_t *, int);
void dequeue(ring_buffer_t *, int *);
void ring_buffer_destroy(ring_buffer_t *);


// producer-consumer
void * producer(void *);
void * consumer(void *);



int main(int argc, char* argv[]){
    ring_buffer_t shared_buffer;
    pthread_t producer_thread;
    pthread_t consumer_thread;

    ring_buffer_create(&shared_buffer);

    pthread_create(&producer_thread, NULL, producer, (void *)&shared_buffer);
    pthread_create(&consumer_thread, NULL, consumer, (void *)&shared_buffer);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    ring_buffer_destroy(&shared_buffer);
    return 0;
}


void ring_buffer_create(ring_buffer_t * ring_buffer){
    static_assert((2 <= BUFFER_SIZE), "BUFFER_SIZE should be at least 2");
    assert((NULL != ring_buffer)&&"Ring buffer argument should not be not");
    ring_buffer->read_head = 0;
    ring_buffer->write_head = 0;
    pthread_mutex_init(&ring_buffer->ring_buffer_mutex, NULL);
    pthread_cond_init(&ring_buffer->is_empty, NULL);
    pthread_cond_init(&ring_buffer->is_full, NULL);
}


int ring_buffer_full(const ring_buffer_t * ring_buffer){
    int next_idx = (ring_buffer->write_head + 1) % BUFFER_SIZE;
    if (next_idx == ring_buffer->read_head) return 1;
    return 0;
}


int ring_buffer_empty(const ring_buffer_t * ring_buffer){
    if (ring_buffer->write_head == ring_buffer->read_head) return 1;
    return 0;
}


void ring_buffer_destroy(ring_buffer_t * ring_buffer){
    ring_buffer->read_head = 0;
    ring_buffer->write_head = 0;
    pthread_mutex_destroy(&ring_buffer->ring_buffer_mutex);
    pthread_cond_destroy(&ring_buffer->is_empty);
    pthread_cond_destroy(&ring_buffer->is_full);
}



void enqueue(ring_buffer_t * ring_buffer, int item){
    pthread_mutex_lock(&ring_buffer->ring_buffer_mutex);
    while (ring_buffer_full(ring_buffer)){
        printf("waiting for a consumer...\n");
        pthread_cond_wait(&ring_buffer->is_empty, &ring_buffer->ring_buffer_mutex);
    }
    ring_buffer->buffer[ring_buffer->write_head] = item;
    ring_buffer->write_head = (ring_buffer->write_head + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&ring_buffer->ring_buffer_mutex);
    printf("finished enqueuing.\n");
    pthread_cond_signal(&ring_buffer->is_full);
    fflush(stdout);
}

void dequeue(ring_buffer_t * ring_buffer, int * item){
    pthread_mutex_lock(&ring_buffer->ring_buffer_mutex);
    while (ring_buffer_empty(ring_buffer)){
        printf("waiting for a producer...\n");
        pthread_cond_wait(&ring_buffer->is_full, &ring_buffer->ring_buffer_mutex);
    }
    *item = ring_buffer->buffer[ring_buffer->read_head];
    ring_buffer->read_head = (ring_buffer->read_head + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&ring_buffer->ring_buffer_mutex);
    printf("finished dequeuing.\n");
    pthread_cond_signal(&ring_buffer->is_empty);
    fflush(stdout);
}

void * producer(void * arg){
    ring_buffer_t * shared_queue = (ring_buffer_t *) arg;
    printf("Hello, I am the producer.\n");
    int some_numbers[] = {3,2,5,6,1}; // some numbers 
    int some_numbers_len = len(int, some_numbers);
    int i = 0;
    while (i < some_numbers_len){
        enqueue(shared_queue, some_numbers[i]);
        i++;
    }
    return NULL;
}

// consumer runs forever lol
void * consumer(void * arg){
    printf("Hello, I am the consumer.\n");
    ring_buffer_t * shared_queue = (ring_buffer_t *) arg;
    while(1){
        int val;
        dequeue(shared_queue, &val);
        printf("doing some processing on task #%d\n", val);
    }
    return NULL;
}
