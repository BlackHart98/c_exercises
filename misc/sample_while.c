#include <stdio.h>

int 
main(void)
{
    char *sample = "hello world!";
    char output[13] = {0};
    int i;

    i = 0;
    while ((output[i] = sample[i] != '\0')){
        printf("item: output[%d] %d\n", i, output[i]); i++;
    }
    printf("=================================\n");
    i = 0;
    while ((output[i] = sample[i]) != '\0'){
        printf("item:: output[%d] %c\n", i, output[i]); i++;
    }
    return 0;
}