// simulating yield-next generator with setjmp.h
// This is just for learning purposes (it's littered with bugs)
#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>
#include <assert.h>


#define MAX_STACK_SIZE 1024



#define generator_init(generator, fn, args, type_) \
    if (!setjmp(generator.current_env)) \
        generator_create(&generator, fn, &args, sizeof(type_), generator.current_env); \


#define next(generator, result) \
    if (!setjmp(generator.current_env)) \
        generator_next(&generator, &result, generator.current_env); 

#define yield(generator, result) \
    jmp_buf new_buf;\
    if (!setjmp(new_buf)) \
        generator_yield(&(*generator), (void *)&result, new_buf); 

#define end_generator(generator) \
    generator->state = 0; \
    if (!generator->state) generator_stop(&(*generator)); \
    else assert(0); 




typedef struct _generator generator_t;


struct _generator{
    unsigned char state;
    int top;
    size_t ret_bytes;
    void * result;
    jmp_buf current_env;
    jmp_buf snapshot[MAX_STACK_SIZE];
};



void generator_create(generator_t *, void * (*func)(generator_t *, void *), void *, size_t, jmp_buf);
void generate(generator_t *, void * (*func)(generator_t *, void *), void *);
void generator_stack_push(generator_t *, jmp_buf);
void generator_stack_pop(generator_t *);
void generator_yield(generator_t *, void *, jmp_buf);
void generator_next(generator_t *, void *, jmp_buf);
void generator_stop(generator_t *);


void * foo(generator_t * generator, void * args){
    int * foobar = (int *) args;
    printf("argument is #%d!\n", *foobar);
    int some_result = *foobar;
    for(int i = 0; i < 3; i++){
        some_result += 1;
        yield(generator, some_result);
    }
    end_generator(generator);
    return NULL;
}

// We have Generators at home
// The generator: 😅...
int main(int argc, char* argv[]){
    generator_t my_generator;
    int some_num = 55;

    generator_init(my_generator, foo, some_num, int);
    
    int result;
    printf("Yielded to the main function.\n");
    next(my_generator, result);

    printf("Yielded to the main function again.\n");
    printf("Result from the generator next = #%d.\n", result);
    next(my_generator, result);

    printf("Result from the generator next next = #%d.\n", result);
    next(my_generator, result);

    printf("Result from the generator next next next = #%d.\n", result);
    return 0;
}

void generator_create(
    generator_t * generator
    , void *func(generator_t *, void *)
    , void * args, size_t ret_bytes
    , jmp_buf base_buf
){
    generator->top = -1;
    generator->state = 1;
    generator->ret_bytes = ret_bytes;
    generator->result = NULL;
    generator_stack_push(&(*generator), base_buf);
    func(&(*generator), &(*args));
    printf("Should come here instead and print this\n");

}


void generator_stack_push(generator_t * generator, jmp_buf item){
    assert(((generator->top + 1) < MAX_STACK_SIZE) && "Generator stack overflow");
    memcpy(generator->snapshot[++(generator->top)], item, sizeof(jmp_buf));
}

void generator_stack_pop(generator_t * generator){
    if ((generator->top - 1) < -1){
        printf("Generator stack underflow\n");
    }
    generator->top--;
}


void generator_yield(generator_t * generator, void * ret, jmp_buf new_buf){
    jmp_buf * old_top; // hold the register ans pointer info before a pushing the called function info to the stack
    old_top = &generator->snapshot[generator->top];// caution here
    generator_stack_push(&(*generator), new_buf);
    generator->result = ret;
    longjmp(*old_top, 1); // jump to the calling function
}

void generator_stop(generator_t * generator){
    printf("Hello you know you got here? top = #%d\n", generator->top);
    longjmp(generator->snapshot[generator->top], 1);
}

void generator_next(generator_t * generator, void * ret, jmp_buf base_buf){
    printf("we are going to the next\n");
    if (!generator->state) 
        assert(0&&"Generator is dead!");
    memcpy(ret, generator->result, generator->ret_bytes);
    jmp_buf * old_top;
    old_top = &generator->snapshot[generator->top];// caution here
    generator_stack_push(&(*generator), base_buf);
    longjmp(*old_top, 1); // jmp to generator function
}