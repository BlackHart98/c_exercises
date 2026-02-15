#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void s1(void){
    printf("Processes man in s1\n");
    return;
}


void spawn_me(void){
    pid_t p = fork();
    printf("Processes man\n");
    if (p == 0){
        s1();
        exit(0);
    }
    return;
}

int main(int argc, char *argv[]){
    spawn_me();
    usleep(1 * 1000);
    printf("Processes man in main\n");
    return 0;
}