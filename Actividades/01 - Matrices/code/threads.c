// matrix multiplication with threads
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// Functions declarations
int **initialize(int n);
void input(int **matrix, int n);
void *bruteForce(void *threadarg);

// Global variables
int **A, **B, **result;
int n, T;

// CMD arguments
int main(int argc, char *argv[])
{
    if (argc >= 3)
    {
        // CPU clock
        struct timeval TSTART, TEND;
        double TIME;

        gettimeofday(&TSTART, NULL);

        // CMD parameters
        n = atoi(argv[1]);
        T = atoi(argv[2]);

        // Threads declaration
        pthread_t *threads;
        threads = (pthread_t *)malloc(T * sizeof(pthread_t));

        // Data input
        A = initialize(n);
        B = initialize(n);
        result = initialize(n);
        input(A, n);
        input(B, n);

        // Threads creation
        for (int i = 0; i < T; i++)
        {
            int *ID = (int *)malloc(sizeof(int));
            *ID = i + 1;

            pthread_create(&threads[i], NULL, bruteForce, (void *)ID);
        }

        // Join threads
        for (int i = 0; i < T; i++)
        {
            pthread_join(threads[i], NULL);
        }

        // Free memory
        free(threads);

        // End clock
        gettimeofday(&TEND, NULL);
        TIME = (TEND.tv_sec - TSTART.tv_sec) * 1000.0;    // sec to ms
        TIME += (TEND.tv_usec - TSTART.tv_usec) / 1000.0; // us to ms
        printf("%.5lf\n", TIME / 1000.0);
    }
    return 0;
}

void *bruteForce(void *threadarg)
{
    // Thread ID
    int ID = *((int *)threadarg);

    // Distribution
    int start, end;
    if (ID % T == 0)
    {
        start = (ID - 1) * (n / T);
        end = ID * (n / T);
    }
    else
    {
        start = (ID - 1) * (n / T);
        end = ID * (n / T) + (n % T);
    }

    // Brute force
    for (int i = start; i < end; i++)
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

    pthread_exit(NULL);
}

int **initialize(int n)
{
    int **matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        matrix[i] = (int *)malloc(n * sizeof(int));
    }
    return matrix;
}

void input(int **matrix, int n)
{
    srand(time(NULL)); // Random seed
                       // Random input
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrix[i][j] = 1 + rand() % 9;
        }
    }
}