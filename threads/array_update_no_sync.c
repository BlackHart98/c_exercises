#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_THREADS 3
#define KB(size_) size_ * 1024
#define len(T, obj) (sizeof(obj)/sizeof(T))

void* partial_inc(void *input_desc);
void* partial_apply_event(void *input_desc);


typedef struct input_subarray_desc{
    int start_idx; 
    int end_idx; 
    int *array;
    int *event_array;
    int *result;
} input_subarray_desc;

int main(int argc, char *argv[]){
    int input_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int result_array[9] = {0};
    int event_array[] = {-1, -2, -1, -3, -1, -4, -1, -2, -1};

    size_t array_len = len(int, input_array);

    input_subarray_desc desc_list[MAX_THREADS] = {0};
    void *alloc_buffer = malloc(KB(1)); // preallocate 1KB
    size_t partition_count = array_len/MAX_THREADS;
    int i = 0;
    for (; i < MAX_THREADS - 1; i++){
        desc_list[i].start_idx = i * partition_count;
        desc_list[i].end_idx = (i * partition_count) + partition_count;
        desc_list[i].array = input_array;
        desc_list[i].event_array = event_array;
        desc_list[i].result = (int *)((uintptr_t)alloc_buffer + (uintptr_t)(i * partition_count * sizeof(int))); // added padding
    }
    desc_list[i].start_idx = i * partition_count;
    desc_list[i].end_idx = array_len;
    desc_list[i].array = input_array;
    desc_list[i].event_array = event_array;
    desc_list[i].result = (int *)((uintptr_t)alloc_buffer + (uintptr_t)(i * partition_count * sizeof(int)));

    pthread_t workers[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++){
        pthread_create(&workers[i], NULL, partial_apply_event, (void *)&desc_list[i]);
    }
    

    for (int i = 0; i < MAX_THREADS; i++){
        pthread_join(workers[i], NULL);
    }
    int *result_ptr = result_array;
    for(int i = 0; i < MAX_THREADS - 1; i++){
        memcpy(result_ptr + (i * partition_count), desc_list[i].result, sizeof(int) * partition_count);
    }
    memcpy(
        result_ptr + ((MAX_THREADS - 1) * partition_count)
        , desc_list[MAX_THREADS - 1].result
        , sizeof(int) * (array_len - desc_list[MAX_THREADS - 2].end_idx));


    for (int i = 0; i < array_len; i++){
        printf("result_array[%d] : %d\n", i, result_array[i]);
    }

    free(alloc_buffer);
    return 0;
}

void* partial_inc(void *input_desc){
    input_subarray_desc *local_desc = (input_subarray_desc *) input_desc;
    for (int i = local_desc->start_idx; i < local_desc->end_idx; i++){
        local_desc->result[i - local_desc->start_idx] = local_desc->array[i] + 1;
    }
    return NULL;
}

void* partial_apply_event(void *input_desc){
    input_subarray_desc *local_desc = (input_subarray_desc *) input_desc;
    for (int i = local_desc->start_idx; i < local_desc->end_idx; i++){
        local_desc->result[i - local_desc->start_idx] = local_desc->array[i] + local_desc->event_array[i];
    }
    return NULL;
}