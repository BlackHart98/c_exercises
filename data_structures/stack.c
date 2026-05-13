#include<stdio.h>
#include<stdlib.h>
#include <string.h> 

#define WSA_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"

#define stack_create(allocator, T, capacity)    stack_array_create(allocator, capacity, sizeof(T))



typedef struct _stack_array_t{
    size_t top;
    size_t capacity;
    size_t nbytes;
    unsigned char* content;
} stack_array_t;


stack_array_t 
stack_array_create(arena_allocator_t*, size_t, size_t);

// void 
// stack_array_destroy(stack_array_t*);

void 
stack_array_push(stack_array_t*, void*);

void* 
stack_array_peek(stack_array_t*);

void 
stack_array_pop(stack_array_t* stack);


int 
main(int argc, char* argv[])
{
    arena_allocator_t gpa = arena_allocator_init_page_default(c_allocator, KB(1));
    stack_array_t s = stack_create(&gpa, float, 10);
    float x = 2.07f;
    
    stack_array_push(&s, &x);
    float* val = stack_array_peek(&s);
    printf("foo hello world => %f\n", *val);
    stack_array_pop(&s);

    if (stack_array_peek(&s) == NULL) printf("Stack is empty\n");
    arena_allocator_deinit(&gpa);
    return 0;
}


stack_array_t 
stack_array_create(arena_allocator_t *allocator, size_t capacity, size_t nbytes)
{
    slice_t content_slice = arena_allocator_alloc_aligned(allocator, (capacity * nbytes), nbytes, DEFAULT_ALIGNMENT);
    return (stack_array_t) {
        .top = -1,
        .capacity = capacity,
        .nbytes = nbytes,
        .content = (unsigned char *) content_slice.ptr,
    };
}


void 
stack_array_push(stack_array_t* stack, void* item)
{
    if (stack->top == stack->capacity) return;
    stack->top++;
    memcpy(stack->content + (stack->top * stack->nbytes), item, stack->nbytes);
}


void* 
stack_array_peek(stack_array_t* stack)
{
    if (stack->top == -1) return NULL;
    return stack->content + (stack->top * stack->nbytes);
}


void 
stack_array_pop(stack_array_t* stack)
{
    if (stack->top == -1) return;
    stack->top--;
}