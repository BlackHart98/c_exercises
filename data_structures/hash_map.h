#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <string.h>


#include "../memory_management/why_so_arena.c"
#define ARRAY_LIST_IMPLEMENTATION
#include "../data_structures/array_list.h"

#define HASH_MAP_LOCAL static

// key: slice_t@char[], value: int
typedef struct key_value_t {
    slice_t key;
    int value;
} key_value_t;

typedef struct hash_map_t {
    array_list_t data;
    // array_list_t entry;
    size_t size;
} hash_map_t;

HASH_MAP_LOCAL hash_map_t 
hash_map_init_capacity(arena_allocator_t *allocator, size_t init_capacity);

HASH_MAP_LOCAL int 
hash_map_put(arena_allocator_t *allocator, hash_map_t *hs, key_value_t key_value);

HASH_MAP_LOCAL int 
hash_map_get(arena_allocator_t *allocator, hash_map_t *hs, slice_t key, int *value);

HASH_MAP_LOCAL size_t 
hash_map_get_hash(slice_t input, size_t size);

HASH_MAP_LOCAL char 
hash_map_contains(hash_map_t *hs, slice_t key);

#ifdef HASH_MAP_IMPLEMENTATION
hash_map_t 
hash_map_init_capacity(arena_allocator_t *allocator, size_t init_capacity)
{
    array_list_t result = array_list_init_capacity(allocator, key_value_t, init_capacity);
    array_list_max_bound_fn(&result);
    return (hash_map_t){ .data = result, .size = 0 };
}


int 
hash_map_put(arena_allocator_t *allocator, hash_map_t *hs, key_value_t key_value)
{
    if (hs->size >= hs->data.capacity) {
        // resize hash map, and move the content over
        assert(0&&"Capacity exceeded!");
    }
    // put item into the hash map
    size_t hash = hash_map_get_hash(key_value.key, hs->data.len);
    int ret = array_list_insert_item_fn(&hs->data, (char *)&key_value, hash);
    if (0 != ret) return 1;
    hs->size += 1;
    return 0;
}


int 
hash_map_get(arena_allocator_t *allocator, hash_map_t *hs, slice_t key, int *value)
{
    if (hash_map_contains(hs, key)){
        size_t hash = hash_map_get_hash(key, hs->data.len);
        key_value_t kv = {0};
        int ret = array_list_get_item_fn(&hs->data, (char *)&kv, hash);
        *value = kv.value;
        return 0;
    }
    printf("Could not get item\n");
    return 1;
}


size_t 
hash_map_get_hash(slice_t input, size_t size)
{
    char *ptr = (char *)input.ptr;
    size_t result = 0;
    for (int i = 0; i < input.len_in_bytes; i++) result += ptr[i] * (2 << i);
    return result % size;
}


char 
hash_map_contains(hash_map_t *hs, slice_t key)
{
    size_t hash = hash_map_get_hash(key, hs->data.len);
    key_value_t kv = {0};
    int ret = array_list_get_item_fn(&hs->data, (char *)&kv, hash);
    if (NULL == kv.key.ptr) return 0;
    if (0 == memcmp(key.ptr, kv.key.ptr, key.len_in_bytes)) return 1;
    return 0;
}

#endif

#endif /* HASH_MAP_H */