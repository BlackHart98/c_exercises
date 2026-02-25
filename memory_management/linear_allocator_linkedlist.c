#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdalign.h>


#include <assert.h>

#define GET_SLICE_LEN(slice, type_)                     slice.len_in_bytes/sizeof(type_)
#define BEGIN_ITR(slice, type_)                         (type_ *)slice.ptr
#define END_ITR(itr, slice, type_)                      itr < (type_ *)slice.ptr + GET_SLICE_LEN(slice, type_)

#define KB(byte)                                        (byte * 1024UL)
#define DEFAULT_ALIGNMENT                               (2 * sizeof(void *))
#define DEFAULT_PAGE_SIZE                               KB(2)
#define arena_alloc(arena, T, len)                      arena_alloc_aligned(arena, len, sizeof(T), DEFAULT_ALIGNMENT)
#define arena_allocator_alloc(arena, T, len)           arena_allocator_alloc_aligned(arena, len, sizeof(T), DEFAULT_ALIGNMENT)
#define arena_allocator_init_page_default(capacity)    arena_allocator_init(capacity, DEFAULT_PAGE_SIZE)


// Slice yippy!
typedef struct slice_t {
    void *ptr;
    size_t len_in_bytes;
} slice_t;


slice_t
make_slice(void *object, size_t len_in_bytes);


slice_t
make_slice(void *object, size_t len_in_bytes)
{
    if (NULL == object) {return (slice_t){0};}
    return (slice_t){
        .ptr = object,
        .len_in_bytes = len_in_bytes,
    };
}


// linear allocator
typedef struct Arena {
    unsigned char* base_address;
    size_t capacity; // capacity in bytes
    size_t offset;
} Arena;


// Linked list with allocator
typedef struct ArenaLinkedNode {
    Arena arena;
    struct ArenaLinkedNode* next;
} ArenaLinkedNode;


typedef struct ArenaAllocator {
    size_t page_size;
    ArenaLinkedNode* linkedlist;
} ArenaAllocator;



Arena arena_init(size_t capacity);
uintptr_t align_forward(uintptr_t ptr, uintptr_t alignment_);
slice_t arena_alloc_aligned(Arena *arena, size_t len, size_t size_, size_t alignment_);
void arena_reset(Arena *arena);
void arena_deinit(Arena *arena);

ArenaAllocator arena_allocator_init(size_t capacity, size_t page_size);
slice_t arena_allocator_alloc_aligned(ArenaAllocator *arena_allocator, size_t len, size_t size_, size_t alignment_);
void arena_allocator_reset(ArenaAllocator *arena_allocator);
void arena_allocator_deinit(ArenaAllocator *arena_allocator);


ArenaAllocator arena_allocator_init(size_t capacity, size_t page_size){
    assert((0 < page_size)&&"Page size should always be greater than zero");
    assert((0 < capacity)&&"Capacity should always be greater than zero");
    if (capacity < page_size){capacity = page_size;}
    ArenaLinkedNode *node = (ArenaLinkedNode *)malloc(sizeof(*node)); // hidden allocation
    Arena new_arena = arena_init(capacity);
    if (NULL == new_arena.base_address) return (ArenaAllocator){};
    *node = (ArenaLinkedNode){
        .arena = new_arena,
        .next = NULL,
    };
    return (ArenaAllocator){
        .page_size = page_size,
        .linkedlist = node,
    };
}


slice_t arena_allocator_alloc_aligned(ArenaAllocator *arena_allocator, size_t len, size_t size_, size_t alignment_){
    assert((NULL != arena_allocator->linkedlist)&&"Arena allocator was not initialized");
    ArenaLinkedNode *current_node = arena_allocator->linkedlist;
    while (NULL != current_node->next){current_node = current_node->next;}
    slice_t result = arena_alloc_aligned(&(current_node->arena), len, size_, alignment_);
    if (!result.ptr){
        printf("Creating new sizeable arena\n");
        ArenaLinkedNode *new_node = malloc(sizeof(*current_node->next)); // hidden allocation
        if (NULL == new_node) return (slice_t){};

        size_t page_allocation = arena_allocator->page_size;
        while (page_allocation < size_ * len) page_allocation += arena_allocator->page_size;
        Arena new_arena = arena_init(page_allocation);
        if (NULL == new_arena.base_address) return (slice_t){};

        *new_node = (ArenaLinkedNode){
            .arena = new_arena,
            .next = NULL,
        };
        current_node->next = new_node;
        return arena_alloc_aligned(&(new_node->arena), len, size_, alignment_);
    }
    return result;
    
}


void arena_allocator_reset(ArenaAllocator *arena_allocator){
    ArenaLinkedNode *current_node = arena_allocator->linkedlist;
    while (NULL != current_node){
        arena_reset(&(current_node->arena));
        current_node = current_node->next;
    }
}


void arena_allocator_deinit(ArenaAllocator *arena_allocator){
    ArenaLinkedNode *current_node = arena_allocator->linkedlist;
    while(NULL != current_node){
        ArenaLinkedNode *temp_next = current_node->next;
        arena_deinit(&(current_node->arena));
        free(current_node);
        current_node = NULL;
        current_node = temp_next;
    }
}


Arena arena_init(size_t capacity){
    void *buf = malloc(capacity);
    if (NULL == buf) {
        return (Arena){};
    } else {   
        return (Arena){
            .base_address = malloc(capacity),
            .capacity = capacity,
            .offset = 0
        };
    }
}


uintptr_t align_forward(uintptr_t ptr, uintptr_t alignment_){
    // Assume the aligment with always be a power of 2
    // check if the current offset is divisible by 2
    assert((0 == (alignment_ & (alignment_ - 1)))&&"Alignment should be in power of 2");
    uintptr_t modulo = ptr & (alignment_ - 1);
    if (0 != modulo) ptr = ptr + alignment_ - modulo;
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
    if (arena->base_address != NULL) free(arena->base_address); // This is a nuke
    arena->base_address = 0;
    arena->capacity = 0;
    arena->offset = 0;
}



int main(int argc,  char* argv[]){
    ArenaAllocator my_arena = arena_allocator_init_page_default(KB(1));

    printf("This is my allocator I'd slap it everywhere possible\n");
    slice_t my_float_1 = arena_allocator_alloc(&my_arena, float, 100000000);
    if (0 < my_float_1.len_in_bytes){
        printf("first init length %lu\n", GET_SLICE_LEN(my_float_1, float));
        float *ptr = (float *)(my_float_1.ptr);
        *ptr = 4.5f;
        printf("get item %.2f\n", ((float *)(my_float_1.ptr))[0]);
    }
    arena_allocator_deinit(&my_arena);
    return 0;
}