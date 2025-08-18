#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define MAX_THREADS 4
#define SENTINEL -1

typedef struct _signal_t{
    int flag;
    int result;
    pthread_mutex_t mutex;
} signal_t;


void signal_create(signal_t *);

void * do_something(void *);


int main(int argc, char * argv[]){
    pthread_t worker_thread[MAX_THREADS];
    signal_t my_signal;
    signal_create(&my_signal);
    for (int i = 0; i < MAX_THREADS; i++){
        printf("hello world! some_signal thread #%d\n", i);
        pthread_create(&worker_thread[i], NULL, do_something, (void *)&my_signal);
    }

    for (int i = 0; i < MAX_THREADS; i++){
        pthread_join(worker_thread[i], NULL);
    }

    if (my_signal.result != SENTINEL){
        printf("Here is the thread succeeded with the result #%d\n", my_signal.result);
    }
    return 0;
}


void * do_something(void * arg){
    assert((arg != NULL)&&"arg should not be NULL");
    signal_t * some_signal = (signal_t *) arg;
    // sleep(2); // doing some work
    pthread_mutex_lock(&(some_signal->mutex));
    if (!some_signal->flag){
        some_signal->result = (int)6;
        some_signal->flag = 1;
        printf("Winner winner chicken dinner!\n");
    } else {
        printf("Damn I couldn't acquire the lock on time\n");
    }
    pthread_mutex_unlock(&(some_signal->mutex));
    return NULL;
    
}

void signal_create(signal_t * my_signal){
    my_signal->flag = 0;
    my_signal->result = SENTINEL;
    pthread_mutex_init(&(my_signal->mutex), NULL);
}

