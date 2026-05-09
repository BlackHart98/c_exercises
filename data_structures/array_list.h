#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <assert.h>


#include "../memory_management/why_so_arena.c"

#define ARRAY_LIST_LOCAL static

#define array_list_init_capacity(allocator, T, len)    array_list_init_capacity_fn(allocator, len, sizeof(T))
 

typedef struct array_list_t{
    const size_t size; // type size
    size_t capacity;
    size_t len;
    char* ptr;
} array_list_t;

ARRAY_LIST_LOCAL array_list_t 
array_list_init_capacity_fn(arena_allocator_t *allocator, size_t init_capacity, size_t size);

ARRAY_LIST_LOCAL int 
array_list_append_item_fn(arena_allocator_t *allocator, array_list_t *dst, char *item);

ARRAY_LIST_LOCAL int 
array_list_append_slice_fn(arena_allocator_t *allocator, array_list_t *dst, slice_t buf);


#ifdef ARRAY_LIST_IMPLEMENTATION

array_list_t 
array_list_init_capacity_fn(arena_allocator_t *allocator, size_t init_capacity, size_t size)
{
    assert((0 < init_capacity)&&"init size should be > 0");
    slice_t string_slice = arena_allocator_alloc_aligned(allocator, (init_capacity * size), size, DEFAULT_ALIGNMENT);
    if(0 == string_slice.len_in_bytes) return (array_list_t){0};
    return (array_list_t){
        .capacity = init_capacity,
        .len = 0,
        .size = size,
        .ptr = (char *)string_slice.ptr
    };
}


// side-effect: Table doubling
int 
array_list_append_item_fn(arena_allocator_t *allocator, array_list_t *dst, char *item)
{
    size_t expected_len = dst->size * (1 + dst->len);
    slice_t new_slice = (slice_t){.len_in_bytes = dst->size * dst->capacity, .ptr = dst->ptr};
    if (0 == new_slice.len_in_bytes) return 1;
    if (dst->capacity < expected_len){
        new_slice = arena_allocator_resize_aligned(allocator, new_slice, expected_len << 1, dst->size, DEFAULT_ALIGNMENT);
        dst->capacity = (1 + dst->len) << 1;
    }
    memmove(&(new_slice.ptr[dst->len]), item, dst->size);
    dst->len += 1;
    dst->ptr = new_slice.ptr;
    return 0;

}


int 
array_list_append_slice_fn(arena_allocator_t *allocator, array_list_t *dst, slice_t buf)
{
    assert((buf.len_in_bytes <= (dst->size * dst->len))&&"incompatible types");
    // size_t expected_len = dst->size * (1 + dst->len);
    // slice_t new_slice = (slice_t){.len_in_bytes = dst->size * dst->capacity, .ptr = dst->ptr};
    // if (0 == new_slice.len_in_bytes) return 1;
    // if (dst->capacity < expected_len){
    //     new_slice = arena_allocator_resize_aligned(arena, new_slice, expected_len << 1, dst->size, DEFAULT_ALIGNMENT);
    //     dst->capacity = (1 + dst->len) << 1
    // }
    // memmove(&(new_slice.ptr[dst->len]), &item, dst->size);
    // dst->len = expected_len;
    // dst->ptr = new_slice.ptr;
    return 0;

}


// int 
// string_lib_append_strlit(arena_allocator_t *allocator, string_t *dst, const char *str_lit)
// {
//     if (NULL == str_lit) return 0;
//     size_t len = strlen(str_lit);
//     size_t expected_len = dst->len + len;
//     slice_t new_slice = (slice_t){.len_in_bytes = dst->capacity, .ptr = dst->ptr};
//     if (0 == new_slice.len_in_bytes) return 1;
//     if (dst->capacity < expected_len){
//         dst->capacity = expected_len << 1;
//         new_slice = arena_allocator_resize(allocator, char, new_slice, dst->capacity);
//     }
//     memmove(&(new_slice.ptr[dst->len]), str_lit, len);
//     dst->len = expected_len;
//     dst->ptr = new_slice.ptr;
//     return 0;

// }


// // side-effect: Table doubling
// int 
// string_lib_append_char(arena_allocator_t *allocator, string_t *dst, const char src_char)
// {
//     size_t expected_len = dst->len + 1;
//     slice_t new_slice = (slice_t){.len_in_bytes = dst->capacity, .ptr = dst->ptr};
//     if (0 == new_slice.len_in_bytes) return 1;
//     if (dst->capacity < expected_len){
//         dst->capacity = expected_len << 1;
//         new_slice = arena_allocator_resize(allocator, char, new_slice, dst->capacity);
//     }
//     memmove(&(new_slice.ptr[dst->len]), &src_char, 1);
//     dst->len = expected_len;
//     dst->ptr = new_slice.ptr;
//     return 0;
// }


// int 
// string_lib_to_cstring(arena_allocator_t *allocator, string_t *string, char **cstring)
// {
//     slice_t cstring_slice = arena_allocator_alloc(allocator, char, string->len + 1);
//     if (0 == cstring_slice.len_in_bytes) return 1;
//     assert((0 != cstring_slice.len_in_bytes)&&"Unable to allocate slice");
//     memset(cstring_slice.ptr, 0, cstring_slice.len_in_bytes);
//     memmove(cstring_slice.ptr, string->ptr, string->len);
//     *cstring = (char *)cstring_slice.ptr;
//     return 0;
// }

#endif

#endif /* ARRAY_LIST_H */