#include <stdio.h>

#define WSA_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#define ARRAY_LIST_IMPLEMENTATION
#include "../data_structures/array_list.h"

int 
main(int argc, char* argv[])
{
    arena_allocator_t gpa = arena_allocator_init_page_default(c_allocator, KB(1));
    array_list_t float_list = array_list_init_capacity(&gpa, float, 200); // float_list: [dynamic]float
    if (NULL != float_list.ptr){
        printf("first init length\n");
        float item = 4.5;
        int ret = array_list_append_item_fn(&gpa, &float_list, (char *)&item);
        if (0 != ret) {
            printf("Empty!\n");
            return 1;
        }
        float *ptr = (float *)(float_list.ptr);
        for (int i = 0; i < float_list.len; i++){
            printf("float_list[%d]: %f\n", i, ptr[i]);
        }
    }
    arena_allocator_deinit(&gpa);
    return 0;
}

