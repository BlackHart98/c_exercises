#include <stdio.h>

#define WSA_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#define ARRAY_LIST_IMPLEMENTATION
#include "../data_structures/array_list.h"

int 
main(int argc, char* argv[])
{
    arena_allocator_t gpa = arena_allocator_init_page_default(c_allocator, KB(1));

    array_list_t float_list = array_list_init_capacity(&gpa, float, 10); // float_list: [dynamic]float
    int ret;
    if (NULL == float_list.ptr) goto cleanup;
    float input_array[] = {8, 9, 10, 4.5, 0.7};

    slice_t buf_slice = make_slice((void *)input_array, sizeof(input_array)); 
    ret = array_list_append_slice_fn(&gpa, &float_list, buf_slice);
    if (0 != ret) goto cleanup;

    float *ptr = (float *)(float_list.ptr);
    for (size_t i = 0; i < float_list.len; i++){
        printf("float_list[%d]: %f\n", i, ptr[i]);
    }

    cleanup:
        arena_allocator_deinit(&gpa);
    return 0;
}

