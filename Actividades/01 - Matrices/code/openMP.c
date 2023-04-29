// mxm matrix multiplication applying OpenMP
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

// Functions declarations
void bruteForce(int **A, int **B, int **result, int n)
{
    int i, j, k;
    #pragma omp parallel for shared(A, B, result) private(i, j, k)
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            result[i][j] = 0;
            for (k = 0; k < n; k++)
            {
                result[i][j] += A[i][k] * B[k][j];
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
    if (argc >= 2)
    {
        srand(time(NULL)); // Random seed

        // CPU clock
        struct timeval TSTART, TEND;
        double TIME;
        gettimeofday(&TSTART, NULL);

        // Matrix declaration
        int n = atoi(argv[1]);
        int **a, **b;

        a = initialize(n);
        b = initialize(n);

        input(a, n);
        input(b, n);

        int **result;
        result = initialize(n);

        // Matrix multiplication with O(n^3) algorithm
        bruteForce(a, b, result, n);

        // End clock
        gettimeofday(&TEND, NULL);
        TIME = (TEND.tv_sec - TSTART.tv_sec) * 1000.0;    // sec to ms
        TIME += (TEND.tv_usec - TSTART.tv_usec) / 1000.0; // us to ms
        printf("%.5lf\n", TIME / 1000.0);

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