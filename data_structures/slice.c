#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define GET_SLICE_LEN(slice, type_)  slice.len_in_bytes/sizeof(type_)
#define BEGIN_ITR(slice, type_) (type_ *)slice.buf
#define END_ITR(itr, slice, type_) itr < (type_ *)slice.buf + GET_SLICE_LEN(slice, type_)



typedef struct slice_t {
    void *buf;
    size_t len_in_bytes;
} slice_t;


typedef struct const_slice_t {
    const void *buf;
    const size_t len_in_bytes;
} const_slice_t;



slice_t
make_slice(void *object, size_t len_in_bytes);



// Does not abort on NULL
slice_t
make_slice_relaxed(void *object, size_t len_in_bytes)
{
    if (!object || len_in_bytes == 0) {return (slice_t){0};}
    return (slice_t){
        .buf = object,
        .len_in_bytes = len_in_bytes,
    };
}


// Does not abort on NULL
const_slice_t
make_const_slice_relaxed(const char *object)
{
    size_t len_in_bytes = 0;
    size_t i = 0;
    if (!object) {return (const_slice_t){0};}
    while ('\0' != object[i]){i++;}
    len_in_bytes = i * sizeof(char) + 1;
    return (const_slice_t){
        .buf = object,
        .len_in_bytes = len_in_bytes,
    };
}



slice_t
make_slice(void *object, size_t len_in_bytes)
{
    assert((NULL != object)&&"object can not be NULL");
    return (slice_t){
        .buf = object,
        .len_in_bytes = len_in_bytes,
    };
}


const_slice_t
make_const_slice(const char *object)
{
    assert((NULL != object)&&"object can not be NULL");
    size_t len_in_bytes = 0;
    size_t i = 0;
    while ('\0' != object[i]){i++;}
    len_in_bytes = i * sizeof(char) + 1;
    return (const_slice_t){
        .buf = object,
        .len_in_bytes = len_in_bytes,
    };
}