// mxm matrix multiplication using processes and shared memory (fork)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h> // Have no fucking idea why with sys/time.h it doesn't work

// Functions declarations
int **initialize(int n);
void input(int **matrix, int n);
void bruteForce(int **A, int **B, int **result, int parameters[3]);

// CMD arguments
int main(int argc, char *argv[])
{
    if (argc >= 3)
    {
        srand(time(NULL));           // Random seed
        pid_t PROCESS_ID = getpid(); // Parent process ID

        // CPU clock
        struct timeval TSTART, TEND;
        double TIME;
        gettimeofday(&TSTART, NULL);

        // Parameters
        int n = atoi(argv[1]);
        int P = atoi(argv[2]);

        // Matrix declaration
        int **a, **b;

        a = initialize(n);
        b = initialize(n);

        input(a, n);
        input(b, n);

        int **result;
        result = initialize(n);

        // Process creation
        pid_t pid = getpid(); // Actual process ID

        // We don't need to create a new process for the last one as we already count the parent process
        for (int i = 0; i < P-1; i++)
        {
            int parameters[3] = {n, P, i + 1}; // Matrix size, number of processes, process ID

            if (pid != 0) // Parent process
            {
                bruteForce(a, b, result, parameters);

                wait(NULL); // Wait for the child process to finish
                pid = fork(); // Create a new child process
            }
            else if(pid == -1) // Error
            {
                printf("Error creating the process.\n");
                return 1;
            }
        }

        // End clock
        gettimeofday(&TEND, NULL);
        TIME = (TEND.tv_sec - TSTART.tv_sec) * 1000.0;    // sec to ms
        TIME += (TEND.tv_usec - TSTART.tv_usec) / 1000.0; // us to ms

        if (getpid() == PROCESS_ID)
        {
            printf("%.5lf\n", TIME / 1000.0);
        }

        // Free memory
        for (int i = 0; i < n; i++)
        {
            free(a[i]);
            free(b[i]);
            free(result[i]);
        }
    }
    else
    {
        printf("You lack of arguments. Please, try again.\n");
        printf("Parameters: <matrix size> <number of processes>\n");
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
    // Random input
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrix[i][j] = 1 + rand() % 9;
        }
    }
}

// Matrix multiplication with O(n^3) algorithm
void bruteForce(int **A, int **B, int **result, int parameters[3])
{
    // Parameters
    int n = parameters[0];  // Matrix size
    int P = parameters[1];  // Number of processes
    int id = parameters[2]; // Process ID

    // Distribute the work
    int start, end;
    if (P == 0) // Avoid division by zero
    {
        P = 1;
    }

    if (id % P == 0)
    {
        start = (id - 1) * (n / P);
        end = id * (n / P);
    }
    else
    {
        start = (id - 1) * (n / P);
        end = id * (n / P) + (n % P);
    }

    // Matrix multiplication
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