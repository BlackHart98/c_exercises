#include <stdio.h>
int g_var = 35;

void
foo()
{
    extern int g_var;
    printf("foo: Hello world: %d\n", g_var);
}

int*
foobar()
{
    static int s_var;
    printf("foobar: Hello world: %d\n", s_var);
    return &s_var;
}


int
main(int argc, char *argv[])
{
    foo();
    int *ptr = foobar();
    printf("main: Hello world: %d\n", *ptr);
    return 0;
}