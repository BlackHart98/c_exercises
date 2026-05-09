#include <stdio.h>

#define WSA_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"


int 
main(int argc, char* argv[])
{
    arena_allocator_t my_arena_allocator = arena_allocator_init_page_default(c_allocator, KB(1024));

    printf("This is my allocator I'd slap it everywhere possible\n");
    slice_t my_float_1 = arena_allocator_alloc(&my_arena_allocator, float, 200); // my_float_1: []float
    if (NULL != my_float_1.ptr){
        printf("first init length %lu\n", GET_SLICE_LEN(my_float_1, float));
        float *ptr = (float *)(my_float_1.ptr);
        *ptr = 4.5f;
        printf("get item %.2f\n", ((float *)(my_float_1.ptr))[0]);

        for (float *ptr = BEGIN_ITR(my_float_1, float); END_ITR(ptr, my_float_1, float); ptr++){
            printf("get float <%.2f>\n", *ptr);
        }
        my_float_1 = arena_allocator_resize(&my_arena_allocator, float, my_float_1, 2);
        printf("===========================\n");
        for (float *ptr = BEGIN_ITR(my_float_1, float); END_ITR(ptr, my_float_1, float); ptr++){
            printf("get float <%.2f>\n", *ptr);
        }
        printf("new float slice length is: #%lu\n", GET_SLICE_LEN(my_float_1, float));
    }
    arena_allocator_deinit(&my_arena_allocator);
    return 0;
}

