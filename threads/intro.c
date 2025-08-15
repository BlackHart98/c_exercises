#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>


void *somefunc(void* param){
    printf("hello world from spawned thread!\n");
    return NULL;
}

int main(int argc, char* argv[]){
    pthread_t first_thread;
    pthread_create(&first_thread, NULL, somefunc, NULL);
    sleep(1); // added a second delay
    printf("hello world from main thread!\n");
    pthread_join(first_thread, NULL);
    return 0;
}