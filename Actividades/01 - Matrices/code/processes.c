// mxm matrix multiplication using processes and shared memory (fork)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h> // pid_t

// Functions declarations
int **initialize(int n);
void input(int **matrix, int n);
void bruteForce(int **A, int **B, int **result, int n);

// CMD arguments
int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        // CPU clock
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // Matrix declaration
        int n = atoi(argv[1]);
        int **a, **b;

        a = initialize(n);
        b = initialize(n);

        input(a, n);
        input(b, n);

        int **result;
        result = initialize(n);

        // fork() returns 0 to the child process
        // and the child process id to the parent process
        pid_t pid = fork();

        if (pid == 0)
        {
            // Child process
            printf("Child process: %d   Parent process: %d \n", getpid(), getppid());
        }
        else if (pid > 0)
        {
            // Parent process
            printf("Parent process: %d   Child process: %d \n", getpid(), pid);
        }
        else
        {
            // Error
            printf("Error creating the process!\n");
        }

        // Matrix multiplication with O(n^3) algorithm
        //bruteForce(a, b, result, n);

        // End clock
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("\nTime elapsed: %f seconds\n", cpu_time_used);
    }
    else
    {
        printf("You must put the matrix size as your first argument!\n");
    }

    return 0;
}

// Matrix initialization
int **initialize(int n)
{
    int **matrix;
    matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        matrix[i] = (int *)malloc(n * sizeof(int));
    }

    return matrix;
}

// Matrix input
void input(int **matrix, int n)
{
    printf("Enter the elements of the matrix:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &matrix[i][j]);
        }
    }
}

// Matrix multiplication with O(n^3) algorithm
void bruteForce(int **A, int **B, int **result, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            result[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}