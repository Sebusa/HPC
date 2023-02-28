// matrix multiplication with threads
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Functions declarations
int **initialize(int n);
void input(int **matrix, int n);
void *bruteForce(void *threadarg);
void printResult(int **matrix, int n);

// Thread structure
struct thread
{
    int ID;
    int **A;
    int **B;
    int **result;
    int n;
    int T;
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
        pthread_t threads[T];
        struct thread *data[T];

        // Matrix input
        int **a, **b, **result;
        a = initialize(n);
        b = initialize(n);
        result = initialize(n);
        input(a, n);
        input(b, n);

        // Threads creation
        for (int i = 0; i < T; i++)
        {
            data[i] = (struct thread *)malloc(sizeof(struct thread));
            data[i]->ID = i + 1;
            data[i]->A = a;
            data[i]->B = b;
            data[i]->result = result;
            data[i]->n = n;
            data[i]->T = T;

            pthread_create(&threads[i], NULL, bruteForce, (void *)data[i]);
        }

        // Join threads
        for (int i = 0; i < T; i++)
        {
            pthread_join(threads[i], NULL);
        }

        // End clock
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("%f\n", cpu_time_used);

        //THreads destruction
        pthread_exit(NULL);
    }
    return 0;
}

void *bruteForce(void *threadarg)
{
    // Thread data
    struct thread *data;
    data = (struct thread *)threadarg;

    int ID = data->ID;
    int **A = data->A;
    int **B = data->B;
    int **result = data->result;
    int n = data->n;
    int T = data->T;

    // Distribution
    int start = (ID - 1) * (n / T);
    int end = ID * (n / T);

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

void printResult(int **matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}
