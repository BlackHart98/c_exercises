#include<stdio.h>
#include<stdlib.h>
#include <string.h> 


#define stack_create(stack_hndl, T, capacity)    stack_array_create(stack_hndl, capacity, sizeof(T))



typedef struct _stack_array_t{
    int top;
    int capacity;
    size_t nbytes;
    void** content;
} stack_array_t;


void stack_array_create(stack_array_t*, int, size_t);
void stack_array_destroy(stack_array_t*);
void stack_array_push(stack_array_t*, void*);
void* stack_array_peek(stack_array_t*);
void stack_array_pop(stack_array_t* stack);


int main(int argc, char* argv[]){
    stack_array_t s;
    float x = 2.07f;
    stack_create(&s, float, 2);
    stack_array_push(&s, &x);
    
    float* val = stack_array_peek(&s);

    printf("foo hello world => %f\n", *val);

    stack_array_pop(&s);

    if (stack_array_peek(&s) == NULL) printf("Stack is empty\n");

    stack_array_destroy(&s);

    return 0;
}


void stack_array_create(stack_array_t* stack, int capacity, size_t nbytes){
    stack->top = -1;
    stack->capacity = capacity;
    stack->nbytes = nbytes;
    stack->content = (void* *) malloc(sizeof(void*) * capacity);
}


void stack_array_destroy(stack_array_t* stack){
    if (stack->content == NULL) return;
    stack->top = -1;
    stack->capacity = 0;
    stack->nbytes = 0;
    free(stack->content);
}


void stack_array_push(stack_array_t* stack, void* item){
    if (stack->top == stack->capacity) return;
    stack->top++;
    stack->content[stack->top] = malloc(stack->nbytes);
    memcpy(&(stack->content[stack->top]), &item, stack->nbytes);
}


void* stack_array_peek(stack_array_t* stack){
    if (stack->top == -1) return NULL;
    return stack->content[stack->top];
}


void stack_array_pop(stack_array_t* stack){
    if (stack->top == -1) return;
    stack->top--;
}