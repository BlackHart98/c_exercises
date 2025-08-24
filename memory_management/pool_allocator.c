#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEFAULT_ALIGNMENT               (2 * sizeof(void *))
#define KB(byte)                        (byte * 1024UL)



typedef struct Pool_Free_Node Pool_Free_Node;
struct Pool_Free_Node {
	Pool_Free_Node *next;
};

typedef struct _Pool{
    unsigned char * buffer;
    size_t capacity;
    size_t chunk_size;

    Pool_Free_Node *head;
} Pool;

uint8_t is_power_of_2(uintptr_t x){
    return 0 == (x & (x - 1));
}

uintptr_t align_forward(uintptr_t ptr, uintptr_t alignment_){
    assert(is_power_of_2(alignment_) && "alignment must be a power of 2");
    uintptr_t modulo = ptr & (alignment_ - 1); // fast mod 2
    if (modulo != 0) ptr += alignment_ - modulo;
    return ptr;
}


uint8_t pool_init(Pool * pool, size_t capacity, size_t chunk_size, size_t chunk_alignment){
    assert((0 != capacity)&&"Capacity should be greater than 1");
    pool->buffer = malloc(capacity);
    if(pool->buffer == NULL) return -1;
    uintptr_t start = align_forward((uintptr_t) pool->buffer, (uintptr_t)chunk_alignment);
    uintptr_t initial_start = (uintptr_t)pool->buffer;
    capacity -= (size_t)(start - initial_start);
    chunk_size = (size_t)align_forward(chunk_size, chunk_alignment);

    assert((chunk_size >= sizeof(Pool_Free_Node *))&&"Chunk size is too small");
    assert((chunk_size <= capacity)&&"Chunk size bigger than capacity");

    pool->capacity = capacity;
    pool->chunk_size = chunk_size;
    pool->head = NULL;

    // create free list
    size_t chunk_count = (size_t)(capacity / chunk_size);
    for (size_t i = 0; i < chunk_count; i++){
        void * ptr = &pool->buffer[i * chunk_size];
        Pool_Free_Node * node = (Pool_Free_Node *) ptr;
        node->next = pool->head;
        pool->head = node;
    }
    return 0;
}


void * pool_alloc(Pool * pool){
    if (pool->head == NULL) return NULL;
    void * allocated_addr = pool->head;
    pool->head = pool->head->next;

    return memset(allocated_addr, 0, pool->chunk_size);
}


void pool_free_all(Pool * pool){
    size_t chunk_count = (size_t)(pool->capacity / pool->chunk_size);
    for (size_t i = 0; i < chunk_count; i++){
        void * ptr = &pool->buffer[i * pool->chunk_size];
        Pool_Free_Node * node = (Pool_Free_Node *) ptr;
        node->next = pool->head;
        pool->head = node;
    }
}

void pool_free(Pool * pool, void * ptr){
    // to be implemented
}

void pool_destroy(Pool * pool){
    if (pool->buffer != NULL){
        free(pool->buffer);
        pool->buffer = NULL;
    }
}

int main(int argc, char* argv[]){
    Pool my_pool;
    pool_init(&my_pool, KB(1), 64, DEFAULT_ALIGNMENT);
    return 0;
}