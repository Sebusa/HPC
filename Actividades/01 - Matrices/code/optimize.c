// mxm matrix multiplication with memory line optimization
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Functions declarations */
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
    // Random input
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrix[i][j] = 1 + rand() % 9;
        }
    }
}

// CMD arguments
int main(int argc, char *argv[])
{
    srand(time(NULL)); // Random seed
    // CPU clock
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    // Matrix declaration
    int n = (argc > 1) ? atoi(argv[1]) : 1000;
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
    
    return 0;
}