#define WSA_IMPLEMENTATION
#define STRING_LIB_IMPLEMENTATION
#define ARRAY_LIST_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#include "../data_structures/array_list.h"
#include "../data_structures/string_lib.h"

int
main(void)
{
    arena_allocator_t gpa = arena_allocator_init_page_default(c_allocator, KB(1));
    if (NULL == gpa.linkedlist) goto cleanup;
    string_t str = string_lib_init_slice(&gpa, make_const_slice("hel/lo/wor/ld!"));
    array_list_t list = string_lib_split_string(&gpa, &str, make_const_slice("/"));

    slice_t *splits = (slice_t *)list.ptr;
    for (size_t i = 0; i < list.len; i++){
        char *cstr = NULL;
        string_lib_slice_to_cstring(&gpa, splits[i], &cstr);
        printf("Token: $%s$\n", cstr);
    }
    cleanup:
        arena_allocator_deinit(&gpa);
    return 0;
}