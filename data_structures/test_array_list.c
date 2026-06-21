#include <stdio.h>

#define WSA_IMPLEMENTATION
#define ARRAY_LIST_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#include "../data_structures/array_list.h"

int 
main(int argc, char* argv[])
{
    context_t context = context_init(MB(1), KB(512)); // I need to fix the API boundary of this object!
    if (NULL == context.allocator.linkedlist || NULL == context.temp_allocator.linkedlist) goto cleanup;

    array_list_t float_list = array_list_init_capacity(&(context.allocator), float, 10); // float_list: [dynamic]float
    int ret;
    if (NULL == float_list.ptr) goto cleanup;
    float input_array[] = {8, 9, 10, 4.5, 0.7};

    slice_t buf_slice = make_slice((void *)input_array, sizeof(input_array)); 
    ret = array_list_append_slice_fn(&(context.allocator), &float_list, buf_slice);
    if (0 != ret) goto cleanup;

    float *ptr = (float *)(float_list.ptr);
    for (size_t i = 0; i < float_list.len; i++){
        printf("float_list[%zu]: %.2f\n", i, ptr[i]);
    }

    cleanup:
        context_deinit(&context);
    return 0;
}

