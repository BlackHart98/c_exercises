#include <stdio.h>

#define WSA_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#define HASH_MAP_IMPLEMENTATION
#include "../data_structures/hash_map.h"
#define STRING_LIB_IMPLEMENTATION
#include "../data_structures/string_lib.h"

int 
main(int argc, char *argv[])
{
    arena_allocator_t gpa = arena_allocator_init_page_default(c_allocator, KB(1));
    hash_map_t some_map = hash_map_init_capacity(&gpa, 200); // float_list: map[char[]]int
    if (0 != some_map.data.capacity){
        string_t str = string_lib_init_with_strlit(&gpa, "test_key");
        int ret = hash_map_put(
            &gpa, &some_map, (key_value_t){ .key = string_lib_to_slice(&str), .value = 95 });
        int result = 0;
        ret = hash_map_get(&gpa, &some_map, string_lib_to_slice(&str), &result);
        char *cstring = NULL;
        ret = string_lib_to_cstring(&gpa, &str, &cstring);
        printf("Here is the result: %d, %s\n", result, cstring);
    }
    arena_allocator_deinit(&gpa);
    return 0;
}
