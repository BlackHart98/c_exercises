#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <string.h>
#include <limits.h>


#include "../memory_management/why_so_arena.c"
#define ARRAY_LIST_IMPLEMENTATION
#include "../data_structures/array_list.h"

#define HASH_MAP_LOCAL static
#define DEFAULT_CAPACITY 500


#define hash_map_init(allocator) hash_map_init_capacity(allocator, DEFAULT_CAPACITY)

enum SLOT_STATE { EMPTY=0, OCCUPIED, DELETED };

typedef enum HASH_MAP_RESULT {
    HM_SUCCESS=0,
    HM_OOM,
    HM_KEY_NOT_IN_MAP,
} hm_result_t;


// key: slice_t@char[], value: int
typedef struct key_value_t {
    slice_t key;
    int value;
    int slot_state;
} key_value_t;

typedef struct hash_map_t {
    slice_t data;
    size_t size;
} hash_map_t;

typedef struct map_entry_t {
    slice_t key;
    int value;
} map_entry_t;

HASH_MAP_LOCAL hash_map_t 
hash_map_init_capacity(arena_allocator_t *allocator, size_t init_capacity);

HASH_MAP_LOCAL hm_result_t 
hash_map_put(arena_allocator_t *allocator, hash_map_t *hs, map_entry_t entry);

HASH_MAP_LOCAL hm_result_t 
hash_map_get(hash_map_t *hs, slice_t key, int *value);

HASH_MAP_LOCAL size_t 
hash_map_get_hash(slice_t input, size_t size);

HASH_MAP_LOCAL char 
hash_map_contains(hash_map_t *hs, slice_t key);

#ifdef HASH_MAP_IMPLEMENTATION
hash_map_t 
hash_map_init_capacity(arena_allocator_t *allocator, size_t init_capacity)
{
    slice_t result = arena_allocator_alloc(allocator, key_value_t, init_capacity);
    key_value_t *ptr = (key_value_t *) result.ptr;
    size_t size = result.len_in_bytes / sizeof(key_value_t);
    for (size_t i = 0; i < size; i++) {
        ptr[i].slot_state = EMPTY;
    }
    return (hash_map_t){ .data = result, .size = 0 };
}


hm_result_t 
hash_map_put(arena_allocator_t *allocator, hash_map_t *hs, map_entry_t entry)
{
    assert((0 != hs->data.len_in_bytes)&&"Empty slice!");
    size_t size = hs->data.len_in_bytes / sizeof(key_value_t);
    if (hs->size >= size) return HM_OOM;
    // put item into the hash map
    key_value_t *ptr = (key_value_t *) hs->data.ptr;
    size_t hash = hash_map_get_hash(entry.key, size);
    size_t tombstone = SIZE_MAX;

    for (size_t i = 0; i < size; i++){
        size_t index = (hash + i) % size;
        key_value_t *key_value = &ptr[index];
        switch (key_value->slot_state){
            case EMPTY: {
                slice_t key_slice = arena_allocator_dup(allocator, entry.key);
                if (0 == key_slice.len_in_bytes) return HM_OOM; 
                ptr[index] = (key_value_t) {
                    .key = key_slice, .slot_state = OCCUPIED, .value = entry.value,};
                hs->size += 1;
                return HM_SUCCESS;
            }
            case OCCUPIED:
                if (slice_equal(&ptr[index].key, &entry.key)) { 
                    ptr[index].value = entry.value; 
                    return HM_SUCCESS; 
                }
                break;
            case DELETED: 
                assert(0&&"Not yet implemented");
                break;
        }
    }
    return HM_SUCCESS;
}


hm_result_t 
hash_map_get(hash_map_t *hs, slice_t key, int *value)
{
    assert((0 != hs->data.len_in_bytes)&&"Empty slice!");
    size_t size = hs->data.len_in_bytes / sizeof(key_value_t);
    key_value_t *ptr = (key_value_t *) hs->data.ptr;
    size_t hash = hash_map_get_hash(key, size);
    for (size_t i = 0; i < size; i++){
        size_t index = (hash + i) % size;
        key_value_t key_value = ptr[index];
        switch (key_value.slot_state) {
            case OCCUPIED:
                if (slice_equal(&ptr[index].key, &key)) { 
                    *value = ptr[index].value;
                    return HM_SUCCESS;
                }
                break;
            case EMPTY:
                return HM_KEY_NOT_IN_MAP;
            case DELETED:
                break;
        }
    }
    return HM_KEY_NOT_IN_MAP;
}


size_t 
hash_map_get_hash(slice_t input, size_t size)
{
    char *ptr = (char *)input.ptr;
    size_t result = 0;
    for (int i = 0; i < input.len_in_bytes; i++) 
        result += (unsigned char)ptr[i] * (2 << i);
    return result % size;
}


char 
hash_map_contains(hash_map_t *hs, slice_t key)
{
    assert((0 != hs->data.len_in_bytes)&&"Empty slice!");
    size_t size = hs->data.len_in_bytes / sizeof(key_value_t);
    size_t hash = hash_map_get_hash(key, size);
    key_value_t *ptr = (key_value_t *) hs->data.ptr;
    for (size_t i = 0; i < size; i++){
        size_t index = (hash + i) % size;
        key_value_t key_value = ptr[index];
        switch (key_value.slot_state) {
            case OCCUPIED:
                if (slice_equal(&ptr[index].key, &key)) { return 1; }
                break;
            case EMPTY:
                return 0;
            case DELETED:
                break;
        }
    }
    return 0;
}

#endif

#endif /* HASH_MAP_H */