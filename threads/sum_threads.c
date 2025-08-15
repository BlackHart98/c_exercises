#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define len(T, obj) (sizeof(obj)/sizeof(T))
#define MAX_THREADS 3

void * partial_sum(void *);

struct start_end {int start_idx; int end_idx; int* array; int result;};


int main(int argc, char* argv[]){
    int list_of_nums[] = {1,2,4,3,2,4,9,4,6,2,1,3};

    pthread_t some_thread[MAX_THREADS];

    const size_t array_size = len(int, list_of_nums);
    printf("Size of the array is %lu\n", array_size);

    const size_t partition_step = array_size/MAX_THREADS;
    printf("partitioned array is %lu\n", partition_step);

    struct start_end foobar[MAX_THREADS] = {0};

    // map the indexes to the thread (this can be simplified)
    for (int i = 0; i < MAX_THREADS - 1; i++){
        foobar[i].start_idx = i * partition_step;
        foobar[i].end_idx = (i * partition_step) + partition_step;
        foobar[i].array = list_of_nums; 
        foobar[i].result = 0;
    }
    foobar[MAX_THREADS - 1].start_idx = (MAX_THREADS - 1) * partition_step;
    foobar[MAX_THREADS - 1].end_idx = array_size;
    foobar[MAX_THREADS - 1].array = list_of_nums;
    foobar[MAX_THREADS - 1].result = 0;

    for (int i = 0; i < MAX_THREADS; i++){
        pthread_create(&some_thread[i], NULL, partial_sum, &foobar[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++){
        pthread_join(some_thread[i], NULL);
    }

    int sum = 0;
    for (int i = 0; i < MAX_THREADS; i++){
        sum += foobar[i].result;
    }
    printf("result of the sum of the array is %d\n", sum);
    return 0;
}

void * partial_sum(void * start_end_idx){
    struct start_end* foo = (struct start_end*) start_end_idx;
    for (int i = foo->start_idx; i < foo->end_idx; i++){
        foo->result += foo->array[i];
    }
    printf("partial sum %d\n", foo->result);
    return NULL;
}