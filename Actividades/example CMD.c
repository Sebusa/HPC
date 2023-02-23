/*
    Exercise with command-line arguments.
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    if (argc == 2)
    {
        int a = atoi(argv[1]);
        printf("The argument supplied is %s \n", argv[1]);
        printf("The argument supplied is %d \n", a);
        printf("The argument supplied is %d \n", argc);
    }
    else if (argc > 2)
    {
        printf("Too many arguments supplied.\n");
    }
    else
    {
        printf("One argument expected.\n");
    }

    return 0;
}
