#include <stdio.h>

#define WSA_IMPLEMENTATION
#define HASH_MAP_IMPLEMENTATION
#define STRING_LIB_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#include "../data_structures/hash_map.h"
#include "../data_structures/string_lib.h"

int 
main(int argc, char *argv[])
{
    context_t context = context_init(KB(2), KB(1)); // I need to fix the API boundary of this object!
    if (NULL == context.allocator.linkedlist || NULL == context.temp_allocator.linkedlist) goto cleanup;
    
    hash_map_t some_map = hash_map_init(&(context.allocator), NULL); // some_map: map[char[]]int
    int ret = HM_SUCCESS;
    if (NULL == some_map.data.ptr) goto cleanup;

    ret = hash_map_put(
        &(context.allocator), &some_map, (map_entry_t){ .key = make_const_slice("test_key"), .value = 95 }); // create a clone of the key in the hashmap
    if (HM_SUCCESS != ret) goto cleanup;

    ret = hash_map_put(
        &(context.allocator), &some_map, (map_entry_t){ .key = make_const_slice("test_key_1"), .value = 567 });
    if (HM_SUCCESS != ret) goto cleanup;
    
    int result = 0;
    ret = hash_map_get(&some_map, make_const_slice("test_key_1"), &result);
    if (HM_SUCCESS != ret) goto cleanup;
    
    char *cstring = NULL;
    ret = string_lib_slice_to_cstring(&(context.allocator), make_const_slice("test_key_1"), &cstring);
    if (0 != ret) goto cleanup;
    printf("Here is the result: %d, %s\n", result, cstring);

    if(!hash_map_contains(&some_map, make_const_slice("who_knows"))){
        printf("Key not found in table\n");
    }
    
    cleanup:
        context_deinit(&context);
    return 0;
}
