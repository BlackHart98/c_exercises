#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define GET_SLICE_LEN(slice, type_)  slice.len_in_bytes/sizeof(type_)
#define BEGIN_ITR(slice, type_) (type_ *)slice.buf
#define END_ITR(itr, slice, type_) itr < (type_ *)slice.buf + GET_SLICE_LEN(slice, type_)



typedef struct slice_t {
    void *buf;
    size_t len_in_bytes;
} slice_t;



slice_t
make_slice(void *object, size_t len_in_bytes);


// Edge case: For string literal, it's length include '\0', you have to promise to never change content in buf of this slice
slice_t
make_const_slice(char *object);


/*
C is a mine field, 😂
Here is a "we have slice at home" implementation in C
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


    char *str_literal = "Hello";
    const slice_t str_slice = make_const_slice(str_literal);
    printf ("=========== Const string iterator size (%lu) ==========\n", str_slice.len_in_bytes);
    for (char *itr = BEGIN_ITR(str_slice, char); END_ITR(itr, str_slice, char); itr++){
        printf ("iterator: %c\n", *itr);
    }


    slice_t oops_slice = make_slice(NULL, chunk_size * sizeof(float));
    printf ("=========== NULL iterator ==========\n");
    for (float *oops = BEGIN_ITR(oops_slice, float); END_ITR(oops, oops_slice, float); oops++){
        printf ("NULL iterator #%.2f\n", *oops);
    }
    return 0;
}


slice_t
make_slice(void *object, size_t len_in_bytes)
{
    assert((NULL  != object) &&"object can not be NULL");
    return (slice_t){
        .buf = object,
        .len_in_bytes = len_in_bytes,
    };
}


slice_t
make_const_slice(char *object)
{
    assert((NULL  != object) &&"object can not be NULL");
    int i = 0;
    while ('\0' != object[i]){i++;}
    size_t len_in_bytes = i * sizeof(char);
    return (slice_t){
        .buf = object,
        .len_in_bytes = len_in_bytes,
    };
}