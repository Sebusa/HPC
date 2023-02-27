// matrix multiplication with threads
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

// Functions declarations
int **initialize(int n);
void input(int **matrix, int n);
void *bruteForce(void *threadarg);

// Thread structure
struct thread
{
    int **A;
    int **B;
    int **result;
    int n;
    int ID;
};

// CMD arguments
int main(int argc, char *argv[])
{
    if (argc >= 3)
    {
        // CPU clock
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // CMD parameters
        int n = atoi(argv[1]);
        int T = atoi(argv[2]);

        // Threads declaration
        pthread_t *threads[T];
        struct thread *data[T];

        // Matrix declaration
        for (int i = 0; i < T; i++)
        {
            data[i] = (struct thread *)malloc(sizeof(struct thread));
            data[i]->ID = i;
            data[i]->A = initialize(n);
            data[i]->B = initialize(n);
            data[i]->result = initialize(n);
            data[i]->n = n;
        }

        // Matrix input
        input(data[0]->A, n);
        input(data[0]->B, n);

        // Threads creation
        for (int i = 0; i < T; i++)
        {
            pthread_create(threads[i], NULL, bruteForce, (void *)data[i]);
        }

        /* Threads join
        for (int i = 0; i < T; i++)
        {
            pthread_join(threads[i], NULL);
        }
        */

        // End clock
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Time: %f \n", cpu_time_used);

        // Free memory
        for (int i = 0; i < T; i++)
        {
            free(data[i]->A);
            free(data[i]->B);
            free(data[i]->result);
            free(data[i]);
        }
        pthread_exit(NULL);
    }
    return 0;
}

void *bruteForce(void *threadarg)
{
    // Thread data
    struct thread *data;
    data = (struct thread *)threadarg;
    int **A = data->A;
    int **B = data->B;
    int **result = data->result;
    int n = data->n;
    int ID = data->ID;

    // Brute force
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

    pthread_exit(NULL);
}

int **initialize(int n)
{
    int **matrix = (int **)calloc(n, sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        matrix[i] = (int *)calloc(n, sizeof(int));
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