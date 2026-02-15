#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main(void){
    fprintf(stdout, "Truncating terminal\n");
    fprintf(stdout, "Hello there!\n");
    sleep(1);
    printf("\033[1A\r"); 
    printf("\033[K");
    fprintf(stdout, "Welcome to terminal!\n");

    // float x = 0.251;
    // double y = 0.251;
    // if (x == 0.251){
    //     printf("Yes they are equal!\n");
    // }
    // if (y == x){
    //     printf("Yes they are equal?\n");
    // }
    return 0;
}