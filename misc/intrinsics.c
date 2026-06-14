#include <stdio.h>

static inline int is_odd(int some_number){
    return some_number & 1;
}


int main(int argc, char *argv[]){
    if (is_odd(5)){
        printf("Number is odd!\n");
    } else {
        printf("Number is even!\n");
    }
    return 0;
}