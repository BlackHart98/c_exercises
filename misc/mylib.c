#ifndef MY_LIB
#define MY_LIB


#include <stdio.h>

int say_hello(void){
    printf("hello world!\n");
    return 1;
}


int say_hello_name(char * name){
    printf("hello world, %s!\n", name);
    return 1;
}


#endif