#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>
#include <assert.h>



#define MAX_STACK_SIZE 1024

typedef enum GENERATOR_STATE_T {
    GENERATOR_START = 0,
    GENERATOR_SUSPENDED,
    GENERATOR_RESUME,
    GENERATOR_DEAD,
} GENERATOR_STATE_T;

typedef struct generator_t{
    GENERATOR_STATE_T state;
    int top;
    size_t return_bytes;
    void *result;
    jmp_buf caller_env;
    jmp_buf callee_env;
} generator_t;

void generator_init(
    generator_t *generator_obj
    , void (*generator_fn)(generator_t *, void *)
    , void *arg
    , size_t return_bytes);
void generator_yield(generator_t *generator_obj, jmp_buf callee_env);
void generator_stack_push(generator_t *generator_obj, jmp_buf item);
void generator_stop(generator_t *generator_obj);
void generator_stack_pop(generator_t *generator_obj);
void generator_next(generator_t *generator_obj, void *ret, jmp_buf base_buf);


void foo(generator_t *generator_obj, void *args){
    // while (1){
        int foobar = *(int *)args;
        printf("Here you go! %d\n", foobar);

        foobar += 1;
        memmove(generator_obj->result, (void *)&foobar, generator_obj->return_bytes);
        generator_obj->state = GENERATOR_SUSPENDED;
        if (!setjmp(generator_obj->callee_env))
            longjmp(generator_obj->caller_env, 1);

        generator_obj->state = GENERATOR_DEAD;
        if (!setjmp(generator_obj->callee_env))
            longjmp(generator_obj->caller_env, 1);
        
    // }
    assert(0&&"Unreachable.");
}

int main(int argc, char *argv[]){
    int foobar = 5;
    generator_t my_gen_obj;
    if (!setjmp(my_gen_obj.caller_env)) 
        generator_init(&my_gen_obj, foo, (void *)&foobar, sizeof(int));

    int result = 0;

    result = *(int *)my_gen_obj.result;
    if (GENERATOR_DEAD == my_gen_obj.state) assert(0&&"Generator is dead");
    my_gen_obj.state = GENERATOR_RESUME;
    if (!setjmp(my_gen_obj.caller_env)){
        longjmp(my_gen_obj.callee_env, 1);
    }

    printf("Here is the result: %d\n", result);
    return 0;
}


void generator_init(
    generator_t *generator_obj
    , void (*generator_fn)(generator_t *, void *)
    , void *arg
    , size_t return_bytes
){
    generator_obj->top = -1;
    generator_obj->state = GENERATOR_START;
    generator_obj->return_bytes = return_bytes;
    generator_obj->result = malloc(return_bytes);
    generator_fn(generator_obj, arg);
}