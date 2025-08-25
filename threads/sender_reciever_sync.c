#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#define MAX_THREADS 8

typedef struct {
    int result;
    int full;
    pthread_cond_t done;
    pthread_cond_t empty;
    pthread_mutex_t lock;
} signal_t;

void signal_init(signal_t * my_signal){
    my_signal->result = -1; // Assuming -1 is a sentinel
    my_signal->full = 0;
    pthread_mutex_init(&my_signal->lock, NULL);
    pthread_cond_init(&my_signal->done, NULL);
    pthread_cond_init(&my_signal->empty, NULL);
}


void signal_destroy(signal_t * my_signal){
    my_signal->result = -1; // Assuming -1 is a sentinel
    pthread_mutex_destroy(&my_signal->lock);
    pthread_cond_destroy(&my_signal->done);
}


void * recieve(void * arg){
    signal_t * some_signal = (signal_t *) arg;

    int count = 0;
    int num_of_recievers = MAX_THREADS;
    while(count < num_of_recievers){
        pthread_mutex_lock(&some_signal->lock);
        while(some_signal->full == 0) 
            pthread_cond_wait(&some_signal->done, &some_signal->lock);
        printf("Receiver thread got notified...#%d\n", count);
        fflush(stdout);
        count++;
        some_signal->full = 0;
        pthread_cond_signal(&some_signal->empty);
        pthread_mutex_unlock(&some_signal->lock);
    }
    printf("Receiver got result = #%d\n", some_signal->result);
    fflush(stdout);
    return NULL;
}

void * send(void * arg){
    signal_t * some_signal = (signal_t *) arg;

    pthread_mutex_lock(&some_signal->lock);
    while(some_signal->full == 1) 
        pthread_cond_wait(&some_signal->empty, &some_signal->lock);
    printf("Thread is doing some thing... send 1\n");
    fflush(stdout);
    some_signal->result = 89;
    some_signal->full = 1;
    pthread_cond_signal(&some_signal->done);
    pthread_mutex_unlock(&some_signal->lock);
    return NULL;
}

void * send2(void * arg){
    signal_t * some_signal = (signal_t *) arg;

    pthread_mutex_lock(&some_signal->lock);
    while(some_signal->full == 1) 
        pthread_cond_wait(&some_signal->empty, &some_signal->lock);
    printf("Thread is doing some thing... send 2\n");
    fflush(stdout);
    some_signal->result = 144;
    some_signal->full = 1;
    pthread_cond_signal(&some_signal->done);
    pthread_mutex_unlock(&some_signal->lock);
    return NULL;
}




// blocking
int poll_reciever(pthread_t * __restrict reciever_thread, signal_t * some_signal){
    pthread_join(*reciever_thread, NULL);
    int result = some_signal->result;
    return result;
}




int main(int argc, char * argv[]){
    pthread_t receiver_thread;
    pthread_t sender_thread[MAX_THREADS];

    signal_t some_signal;
    signal_init(&some_signal);

    for (int i = 0; i < MAX_THREADS; i++){
        pthread_create(&sender_thread[i], NULL, send, (void *)&some_signal);
    }

    pthread_create(&receiver_thread, NULL, recieve, (void *)&some_signal);

    printf("Main thread with the result = #%d.\n", poll_reciever(&receiver_thread, &some_signal));
    signal_destroy(&some_signal);
    return 0;
}