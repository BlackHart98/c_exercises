#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdalign.h>

#define KB(byte)                        (byte * 1024)
#define arena_alloc(arena, T, len)      arena_alloc_aligned(arena, len, sizeof(T), _Alignof(T))


// linear allocator
typedef struct {
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
    // assert(is_power_of_two(alignment_));
    // check if the current offset is divisible by 2
    uintptr_t modulo = ptr & (alignment_ - 1);
    if (modulo != 0) ptr = ptr + alignment_ - modulo;
    return ptr;
}

void * arena_alloc_aligned(Arena *arena, size_t len, size_t size_, size_t alignment_){
    uintptr_t curr_offset = (uintptr_t)arena->base_address + (uintptr_t)arena->offset;
    uintptr_t offset = align_forward(curr_offset, alignment_) - (uintptr_t) arena->base_address;
    // the we check if the arena can contain new item(s)
    if ((offset + (len*size_)) <= arena->capacity){
        void *allocated = &arena->base_address[offset];
        arena->offset = offset + (len * size_);
        memset(allocated, 0, len * size_);
        return allocated;
    }
    return NULL;
}


void arena_reset(Arena *arena){
    arena->offset = 0;
}


void arena_deinit(Arena *arena){
    if (arena->base_address != NULL) free(arena->base_address);
    arena->capacity = 0;
    arena->offset = 0;
}

int main(int argc,  char* argv[]){
    Arena my_arena = arena_init(KB(1));
    float* my_float_1 = arena_alloc(&my_arena, float, 10);
    int* my_int_2 = arena_alloc(&my_arena, int, 1);
    printf("base address %p\n", my_arena.base_address);
    printf("first init %p\n", my_float_1);
    printf("second init %p\n", my_int_2);

    my_float_1[0] = 4.5f;

    printf("get item %f\n", my_float_1[0]);
    printf("get int item %d\n", my_int_2[0]);
    return 0;
}