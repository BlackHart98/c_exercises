#include <stdio.h>

inline int is_odd(int some_number){
    return (some_number & 0x01);
}


int main(int argc, char *argv[]){
    if (is_odd(5)){
        printf("Number is odd!\n");
    } else {
        printf("Number is even!\n");
    }
    return 0;
}