// mxm matrix multiplication
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Functions declarations
int **initialize(int n);
void input(int **matrix, int n);
void bruteForce(int **A, int **B, int **result, int n);
void printResult(int **matrix, int n);

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

        // Matrix multiplication with O(n^3) algorithm
        bruteForce(a, b, result, n);

        if (argc == 3)
        {
            if (strcmp(argv[2], "print") == 0)
            {
                printf("\nThe matrix A is:\n");
                printResult(a, n);

                printf("\nThe matrix B is:\n");
                printResult(b, n);

                printf("\nThe result is:\n");
                printResult(result, n);
            }
            else
            {
                printf("You must put 'print' as your printer argument!\n");
            }
        }

        // End clock
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("%f\n", cpu_time_used);

        //Free memory
        for (int i = 0; i < n; i++)
        {
            free(a[i]);
            free(b[i]);
            free(result[i]);
        }
    }
    return 0;
}

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
