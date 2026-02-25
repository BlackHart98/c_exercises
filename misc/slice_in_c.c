#include <stdio.h>
#include <stdlib.h>

#define GET_SLICE_LEN(slice, type_)  slice.len_in_bytes/sizeof(type_)
#define BEGIN_ITR(slice, type_) (type_ *)slice.buf
#define END_ITR(itr, slice, type_) itr < (type_ *)slice.buf + GET_SLICE_LEN(slice, type_)



typedef struct slice_t {
    void *buf;
    size_t len_in_bytes;
} slice_t;



slice_t
make_slice(void *array, size_t len_in_bytes);


/*
C is a mine field, 😂
Here is a "we have slice" implementation in C
*/
int
main (int argc, char **argv)
{
    printf("Implementing slice in C!\n");
    int my_array[5] = {1, 2, 3, 4, 5}; 

    printf("============================\n");
    slice_t my_slice2 = make_slice((void *)my_array, sizeof(my_array));
    printf("============================\n");

    for (int i = 0; i < GET_SLICE_LEN(my_slice2, int); i++){
        printf ("Item slice index #%i\n", (*((int *)my_slice2.buf ) + i));
    }

    int *ptr = BEGIN_ITR(my_slice2, int);
    for (; END_ITR(ptr, my_slice2, int); ptr++){
        printf ("Item slice iterator #%i\n", *ptr);
    }

    size_t chunk_size = 10;
    void *some_heap_mem = calloc(chunk_size, sizeof(float));
    slice_t ya_slice = make_slice(some_heap_mem, chunk_size * sizeof(float));

    float *f_ptr = BEGIN_ITR(ya_slice, float);
    for (; END_ITR(f_ptr, ya_slice, float); f_ptr++){
        printf ("Item slice iterator #%.2f\n", *f_ptr);
    }

    free(ya_slice.buf); // Caution this isn't how it should be used
    return 0;
}


slice_t
make_slice(void *array, size_t len_in_bytes)
{
    return (slice_t){
        .buf = array,
        .len_in_bytes = len_in_bytes,
    };
}