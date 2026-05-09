#define STRING_LIB_IMPLEMENTATION

#ifndef STRING_LIB_H
#define STRING_LIB_H

#include <assert.h>


#include "why_so_arena.h"

#define STRING_LIB_LOCAL static

typedef struct string_t{
    size_t capacity;
    size_t len;
    char* ptr;
} string_t;

STRING_LIB_LOCAL string_t 
string_lib_init_capacity(arena_allocator_t *allocator, size_t init_size);

STRING_LIB_LOCAL string_t 
string_lib_init_with_strlit(arena_allocator_t *allocator, const char *str_lit);

STRING_LIB_LOCAL int 
string_lib_append_string(arena_allocator_t *allocator, string_t *dst, string_t *src);

STRING_LIB_LOCAL int 
string_lib_append_strlit(arena_allocator_t *allocator, string_t *dst, const char *str_lit);

STRING_LIB_LOCAL int 
string_lib_append_char(arena_allocator_t *allocator, string_t *dst, const char src_char);

STRING_LIB_LOCAL int 
string_lib_to_cstring(arena_allocator_t *allocator, string_t *string, char **cstring);


#ifdef STRING_LIB_IMPLEMENTATION

string_t 
string_lib_init_capacity(arena_allocator_t *allocator, size_t init_size)
{
    assert((0 < init_size)&&"init size should be > 0");
    slice_t string_slice = arena_allocator_alloc(allocator, char, init_size);
    return (string_t){
        .capacity = init_size,
        .len = 0,
        .ptr = (char *)string_slice.ptr
    };
}

// the literal being passed is cloned into the string object
string_t 
string_lib_init_with_strlit(arena_allocator_t *allocator, const char *str_lit)
{
    size_t str_len = strlen(str_lit);
    size_t init_size = (str_len << 1);
    
    slice_t string_slice = arena_allocator_alloc(allocator, char, init_size);
    if (0 == string_slice.len_in_bytes) return (string_t){0};
    memcpy(string_slice.ptr, str_lit, str_len);

    return (string_t){
        .capacity = string_slice.len_in_bytes,
        .len = 0,
        .ptr = (char *)string_slice.ptr
    };
}


// side-effect: Table doubling
int 
string_lib_append_string(arena_allocator_t *allocator, string_t *dst, string_t *src)
{
    size_t len = src->len;
    size_t expected_len = dst->len + src->len;
    slice_t new_slice = (slice_t){.len_in_bytes = dst->capacity, .ptr = dst->ptr};
    if (0 == new_slice.len_in_bytes) return 1;
    if (dst->capacity < expected_len){
        dst->capacity = expected_len << 1;
        new_slice = arena_allocator_resize(allocator, char, new_slice, dst->capacity);
    }
    memmove(&(new_slice.ptr[dst->len]), src->ptr, len);
    dst->len = expected_len;
    dst->ptr = new_slice.ptr;
    return 0;

}


int 
string_lib_append_strlit(arena_allocator_t *allocator, string_t *dst, const char *str_lit)
{
    if (NULL == str_lit) return 0;
    size_t len = strlen(str_lit);
    size_t expected_len = dst->len + len;
    slice_t new_slice = (slice_t){.len_in_bytes = dst->capacity, .ptr = dst->ptr};
    if (0 == new_slice.len_in_bytes) return 1;
    if (dst->capacity < expected_len){
        dst->capacity = expected_len << 1;
        new_slice = arena_allocator_resize(allocator, char, new_slice, dst->capacity);
    }
    memmove(&(new_slice.ptr[dst->len]), str_lit, len);
    dst->len = expected_len;
    dst->ptr = new_slice.ptr;
    return 0;

}


// side-effect: Table doubling
int 
string_lib_append_char(arena_allocator_t *allocator, string_t *dst, const char src_char)
{
    size_t expected_len = dst->len + 1;
    slice_t new_slice = (slice_t){.len_in_bytes = dst->capacity, .ptr = dst->ptr};
    if (0 == new_slice.len_in_bytes) return 1;
    if (dst->capacity < expected_len){
        dst->capacity = expected_len << 1;
        new_slice = arena_allocator_resize(allocator, char, new_slice, dst->capacity);
    }
    memmove(&(new_slice.ptr[dst->len]), &src_char, 1);
    dst->len = expected_len;
    dst->ptr = new_slice.ptr;
    return 0;
}


int 
string_lib_to_cstring(arena_allocator_t *allocator, string_t *string, char **cstring)
{
    slice_t cstring_slice = arena_allocator_alloc(allocator, char, string->len + 1);
    if (0 == cstring_slice.len_in_bytes) return 1;
    assert((0 != cstring_slice.len_in_bytes)&&"Unable to allocate slice");
    memset(cstring_slice.ptr, 0, cstring_slice.len_in_bytes);
    memmove(cstring_slice.ptr, string->ptr, string->len);
    *cstring = (char *)cstring_slice.ptr;
    return 0;
}

#endif

#endif /* STRING_LIB_H */