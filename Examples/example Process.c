#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> // pid_t

int main()
{
    // caculate how many process are running
    // If I wanna have a cicle to create processes, it'll be alway run at least 2 processes.
    // pid = fork() and the main process
    pid_t pid = getppid();
    int n = 3;
    for (int i = 0; i < n-1; i++)
    {
        if (pid != 0)
        {
            pid = fork();
        }
    }
    printf("Hello World\n");

    return 0;
}