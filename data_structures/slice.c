#ifndef SLICE_H
#define SLICE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define SLICE_LOCAL static
#define SLICE_PUB 

// Slice utils
#define GET_SLICE_LEN(slice, type_)                     slice.len_in_bytes/sizeof(type_)
#define BEGIN_ITR(slice, type_)                         (type_ *)slice.ptr
#define END_ITR(itr, slice, type_)                      itr < (type_ *)slice.ptr + GET_SLICE_LEN(slice, type_)
#define BOUNDS(slice, type_)                            (type_ *)slice.ptr + GET_SLICE_LEN(slice, type_)



typedef struct slice_t {
    void *ptr;
    size_t len_in_bytes;
} slice_t;

typedef struct const_slice_t {
    const void *buf;
    const size_t len_in_bytes;
} const_slice_t;





SLICE_LOCAL slice_t
make_slice(void *object, size_t len_in_bytes);

SLICE_LOCAL slice_t
make_const_slice(char *object);

SLICE_LOCAL const_slice_t
make_const_slice_v1(const char *object);


#ifdef SLICE_IMPLEMENTATION 
slice_t
make_slice(void *object, size_t len_in_bytes)
{
    if (NULL == object) {return (slice_t){0};}
    return (slice_t){
        .ptr = object,
        .len_in_bytes = len_in_bytes,
    };
}


const_slice_t
make_const_slice_v1(const char *object)
{
    size_t len_in_bytes = 0;
    size_t i = 0;
    if (NULL == object) {return (const_slice_t){0};}
    while ('\0' != object[i]){i++;}
    len_in_bytes = i * sizeof(char);
    return (const_slice_t){
        .buf = object,
        .len_in_bytes = len_in_bytes,
    };
}


slice_t
make_const_slice(char *object)
{
    size_t len_in_bytes = 0;
    size_t i = 0;
    if (NULL == object) {return (slice_t){0};}
    while ('\0' != object[i]){i++;}
    len_in_bytes = i * sizeof(char);
    return (slice_t){
        .ptr = object,
        .len_in_bytes = len_in_bytes,
    };
}


int
slice_equal(const slice_t *lhs, const slice_t *rhs)
{
    if (lhs->len_in_bytes != rhs->len_in_bytes) return 0;
    if (0 == memcmp(lhs->ptr, rhs->ptr, lhs->len_in_bytes)) return 1;
    return 0;
}
#endif

#endif