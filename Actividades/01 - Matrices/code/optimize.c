// mxm matrix multiplication with memory optimization
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Functions declarations
int **initialize(int n);
void input(int **matrix, int n);
void bruteForce(int **A, int **B, int **result, int n);
int **transpose(int **matrix, int n);

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

        b = transpose(b, n);

        input(a, n);
        input(b, n);

        int **result;
        result = initialize(n);

        // Matrix multiplication with O(n^3) algorithm
        bruteForce(a, b, result, n);

        // End clock
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("%f\n", cpu_time_used);

        // Free memory
        for (int i = 0; i < n; i++)
        {
            free(a[i]);
            free(b[i]);
            free(result[i]);
        }
    }
    return 0;
}

// Transpose matrix
int **transpose(int **matrix, int n)
{
    int **transposed = initialize(n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            transposed[i][j] = matrix[j][i];
        }
    }
    return transposed;
}

// algorithm row by row. Apply the memory optimization to avoid cache misses
void bruteForce(int **A, int **B, int **result, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            result[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                result[i][j] += A[i][k] * B[i][k];
            }
        }
    }
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