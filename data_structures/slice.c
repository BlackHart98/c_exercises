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

char JUNK_MEMORY = 0;


slice_t
make_slice(void *object, size_t len_in_bytes);


slice_t
make_slice(void *object, size_t len_in_bytes)
{
    assert((NULL  != object) &&"object can not be NULL");
    return (slice_t){
        .buf = object,
        .len_in_bytes = len_in_bytes,
    };
}