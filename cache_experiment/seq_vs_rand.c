#include <stdio.h>
#include <time.h>


int main(int argc, char * argv[]){
    int some_array[12] = {1,3,5,6,4,25,6,3,24,56,4,245};

    int seq_idx[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
    int rand_idx[12] = {8,1,3,2,4,7,6,5,0,11,10,9};

    clock_t tick;
    
    tick = clock();
    // sequential access
    for (int i = 0; i < 12; i++){
        int some_num = some_array[seq_idx[i]];
    }
    tick = clock() - tick;
    printf("Sequential access: %lu\n", tick);


    tick = clock();
    // random access
    for (int i = 0; i < 12; i++){
        int some_num = some_array[rand_idx[i]];
    }
    tick = clock() - tick;
    printf("Random access: %lu\n", tick);

    return 0;
}