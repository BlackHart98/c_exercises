#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define MAX_THREADS 8


void * say_hello(void*);

int main(int argc, char* argv[]){
    pthread_t some_thread[MAX_THREADS];
    int thread_idx[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++){
        thread_idx[i] = i;
        pthread_create(&some_thread[i], NULL, say_hello, &thread_idx[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++){
        pthread_join(some_thread[i], NULL);
    }
    return 0;
}

void * say_hello(void* thread_idx){
    printf("Hello from thread #%d\n", *((int *) thread_idx));
    return NULL;
}