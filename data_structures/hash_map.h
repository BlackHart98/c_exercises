#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <string.h>
#include <limits.h>


#include "../memory_management/why_so_arena.c"
#define ARRAY_LIST_IMPLEMENTATION
#include "../data_structures/array_list.h"

#define HASH_MAP_LOCAL static
#define DEFAULT_CAPACITY ((1 << 16) - 1)


#define hash_map_init(allocator, hash_fn) hash_map_init_capacity(allocator, DEFAULT_CAPACITY, hash_fn)

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

typedef size_t (*hash_fn_t)(slice_t input, size_t size); 

typedef struct hash_map_t {
    slice_t data;
    size_t size;
    hash_fn_t hash_fn;
} hash_map_t;

typedef struct map_entry_t {
    slice_t key;
    int value;
} map_entry_t;

HASH_MAP_LOCAL hash_map_t 
hash_map_init_capacity(arena_allocator_t *allocator, size_t init_capacity, hash_fn_t hash_fn);

HASH_MAP_LOCAL hm_result_t 
hash_map_put(arena_allocator_t *allocator, hash_map_t *hs, map_entry_t entry);

HASH_MAP_LOCAL hm_result_t 
hash_map_get(hash_map_t *hs, slice_t key, int *value);

HASH_MAP_LOCAL size_t 
hash_map_get_hash(slice_t input, size_t size);

HASH_MAP_LOCAL char 
hash_map_contains(hash_map_t *hs, slice_t key);

HASH_MAP_LOCAL hm_result_t
hash_map_del(hash_map_t *hs, slice_t key);

HASH_MAP_LOCAL hash_map_t
hash_map_resize(arena_allocator_t *allocator, hash_map_t *hs, size_t new_capacity);

HASH_MAP_LOCAL hm_result_t
hash_map_insert(arena_allocator_t *allocator, hash_map_t *hs, slice_t key, int value, size_t size);

#ifdef HASH_MAP_IMPLEMENTATION
hash_map_t 
hash_map_init_capacity(arena_allocator_t *allocator, size_t init_capacity, hash_fn_t hash_fn)
{
    slice_t result = arena_allocator_alloc(allocator, key_value_t, init_capacity);
    key_value_t *ptr = (key_value_t *) result.ptr;
    size_t size = result.len_in_bytes / sizeof(key_value_t);
    for (size_t i = 0; i < size; i++) {
        ptr[i].slot_state = EMPTY;
    }
    if (NULL == hash_fn) hash_fn = hash_map_get_hash;
    return (hash_map_t){ .data = result, .size = 0, .hash_fn = hash_fn};
}


hm_result_t 
hash_map_put(arena_allocator_t *allocator, hash_map_t *hs, map_entry_t entry)
{
    assert((0 != hs->data.len_in_bytes)&&"Empty slice!");
    size_t size = hs->data.len_in_bytes / sizeof(key_value_t);
    if (hs->size >= size) {
        *hs = hash_map_resize(allocator, hs, size * 2);
        if (0 == hs->data.len_in_bytes) return HM_OOM;
        size = hs->data.len_in_bytes / sizeof(key_value_t);
    }

    slice_t key_slice = arena_allocator_dup(allocator, entry.key);
    if (0 == key_slice.len_in_bytes) return HM_OOM; 
    return hash_map_insert(allocator, hs, key_slice, entry.value, size);
}


hm_result_t 
hash_map_get(hash_map_t *hs, slice_t key, int *value)
{
    assert((0 != hs->data.len_in_bytes)&&"Empty slice!");
    size_t size = hs->data.len_in_bytes / sizeof(key_value_t);
    key_value_t *ptr = (key_value_t *) hs->data.ptr;
    size_t hash = hs->hash_fn(key, size);
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
    size_t hash = hs->hash_fn(key, size);
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


hm_result_t
hash_map_del(hash_map_t *hs, slice_t key)
{
    assert((0 != hs->data.len_in_bytes)&&"Empty slice!");
    size_t size = hs->data.len_in_bytes / sizeof(key_value_t);
    size_t hash = hs->hash_fn(key, size);
    key_value_t *ptr = (key_value_t *) hs->data.ptr;
    for (size_t i = 0; i < size; i++){
        size_t index = (hash + i) % size;
        switch (ptr[index].slot_state) {
            case OCCUPIED:
                if (slice_equal(&ptr[index].key, &key)) { 
                    ptr[index].slot_state = DELETED;
                    hs->size -= 1;
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


hash_map_t
hash_map_resize(arena_allocator_t *allocator, hash_map_t *hs, size_t new_capacity)
{
    assert((0 != hs->data.len_in_bytes)&&"Empty slice!");
    assert((hs->size < new_capacity)&&"New capacity should be greater than hash map size");
    size_t size = hs->data.len_in_bytes / sizeof(key_value_t);
    key_value_t *ptr = (key_value_t *) hs->data.ptr;
    hash_map_t new_map = hash_map_init_capacity(allocator, new_capacity, hs->hash_fn);
    if (NULL == new_map.data.ptr) return (hash_map_t){0};
    for (size_t i = 0; i < size; i++){
        switch (ptr[i].slot_state) {
            case OCCUPIED:{
                hm_result_t ret = hash_map_insert(
                    allocator, hs, ptr[i].key, ptr[i].value, size);
                if (HM_SUCCESS != ret) return (hash_map_t){0};
                break;
            }
            case EMPTY:
                break;
            case DELETED:
                break;
        }
    }
    assert((new_map.size == hs->size)&&"Fatal error");
    return new_map;
}


HASH_MAP_LOCAL hm_result_t
hash_map_insert(arena_allocator_t *allocator, hash_map_t *hs, slice_t key, int value, size_t size)
{
    key_value_t *ptr = (key_value_t *) hs->data.ptr;
    size_t hash = hs->hash_fn(key, size);
    size_t tombstone = SIZE_MAX; // sentinel

    for (size_t i = 0; i < size; i++){
        size_t index = (hash + i) % size;
        key_value_t *key_value = &ptr[index];
        switch (key_value->slot_state){
            case EMPTY: {
                ptr[index] = (key_value_t) {
                    .key = key, .slot_state = OCCUPIED, .value = value,};
                hs->size += 1;
                return HM_SUCCESS;
            }
            case OCCUPIED:
                if (slice_equal(&ptr[index].key, &key)) { 
                    ptr[index].value = value; 
                    return HM_SUCCESS; 
                }
                break;
            case DELETED: 
                if (tombstone == SIZE_MAX) tombstone = index;
                break;
        }
    }
    if (tombstone != SIZE_MAX){
        ptr[tombstone] = (key_value_t) {
            .key = key, .slot_state = OCCUPIED, .value = value,};
        hs->size += 1;
        return HM_SUCCESS;
    }
    return HM_OOM;
}

#endif

#endif /* HASH_MAP_H */