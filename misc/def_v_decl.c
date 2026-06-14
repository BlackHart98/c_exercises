#include <stdio.h>



int
main(void)
{
    void
    foobar(int);

    foobar(400);
    return 0;
}


void
foobar(int x)
{
    printf("Hello world %d\n", x);
}