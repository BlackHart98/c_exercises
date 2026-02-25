#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdalign.h>


#include <assert.h>

#define GET_SLICE_LEN(slice, type_)  slice.len_in_bytes/sizeof(type_)
#define BEGIN_ITR(slice, type_) (type_ *)slice.ptr
#define END_ITR(itr, slice, type_) itr < (type_ *)slice.ptr + GET_SLICE_LEN(slice, type_)

#define KB(byte)                        (byte * 1024UL)
#define DEFAULT_ALIGNMENT               (2 * sizeof(void *))
#define arena_alloc(arena, T, len)      arena_alloc_aligned(arena, len, sizeof(T), DEFAULT_ALIGNMENT)

// Slice yippy!
typedef struct slice_t {
    void *ptr;
    size_t len_in_bytes;
} slice_t;


slice_t
make_slice(void *object, size_t len_in_bytes);


// linear allocator
typedef struct Arena {
    unsigned char* base_address;
    size_t capacity; // capacity in bytes
    size_t offset;
} Arena;


Arena arena_init(size_t capacity){
    return (Arena){
        .base_address = malloc(capacity),
        .capacity = capacity,
        .offset = 0
    };
}




uintptr_t align_forward(uintptr_t ptr, uintptr_t alignment_){
    // Assume the aligment with always be a power of 2
    // check if the current offset is divisible by 2
    uintptr_t modulo = ptr & (alignment_ - 1);
    if (modulo != 0) ptr = ptr + alignment_ - modulo;
    return ptr;
}



slice_t arena_alloc_aligned(Arena *arena, size_t len, size_t size_, size_t alignment_){
    uintptr_t curr_offset = (uintptr_t)arena->base_address + (uintptr_t)arena->offset;
    uintptr_t offset = align_forward(curr_offset, alignment_) - (uintptr_t) arena->base_address;
    // the we check if the arena can contain new item(s)
    if ((offset + (len*size_)) <= arena->capacity){
        void *allocated = &arena->base_address[offset];
        arena->offset = offset + (len * size_);
        memset(allocated, 0, len * size_);
        return make_slice(allocated, len * size_);
    }
    return make_slice(NULL, 0);
}



void arena_reset(Arena *arena){
    arena->offset = 0;
}


void arena_deinit(Arena *arena){
    if (arena->base_address != NULL) free(arena->base_address);
    arena->capacity = 0;
    arena->offset = 0;
}


slice_t
make_slice(void *object, size_t len_in_bytes)
{
    if (NULL == object) {return (slice_t){0};}
    return (slice_t){
        .ptr = object,
        .len_in_bytes = len_in_bytes,
    };
}



int main(int argc,  char* argv[]){
    Arena my_arena = arena_init(KB(1));
    slice_t my_float_1 = arena_alloc(&my_arena, float, 10);
    slice_t my_int_2 = arena_alloc(&my_arena, int, 1);
    printf("base address %p\n", my_arena.base_address);
    printf("first init length %lu\n", GET_SLICE_LEN(my_float_1, float));
    printf("second init le %p\n", my_int_2.ptr);


    float *ptr = (float *)(my_float_1.ptr);
    *ptr = 4.5f;

    printf("get item %.2f\n", ((float *)(my_float_1.ptr))[0]);
    printf("get int item %d\n", ((int *)(my_int_2.ptr))[0]);
    return 0;
}