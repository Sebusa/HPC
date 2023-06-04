#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

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

// Algorithm row by row. Apply the memory optimization to avoid cache misses
void bruteForce(int **A, int **B, int **result, int n, int start_row, int end_row)
{
    for (int i = start_row; i < end_row; i++)
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

int main(int argc, char *argv[])
{
    srand(time(NULL)); // Random seed

    int **a, **b, **result;                    // Matrixes
    int n = (argc > 1) ? atoi(argv[1]) : 1000; // Matrix size

    // Allocate memory for matrices
    a = initialize(n);
    b = initialize(n);
    result = initialize(n);

    input(a, n);
    input(b, n);
    b = transpose(b, n);

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get the rank of the current process and the total number of processes
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // CPU clock
    double start_time, end_time;
    start_time = MPI_Wtime();

    // Broadcast matrix B to all processes
    MPI_Bcast(&b[0][0], n * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate the number of rows each process will handle
    int rows_per_process = n / size;
    int start_row = rank * rows_per_process;
    int end_row = start_row + rows_per_process;

    // Last process takes care of any remaining rows
    if (rank == size - 1 && end_row < n)
    {
        end_row = n;
    }

    // Each process multiplies its assigned rows
    bruteForce(a, b, result, n, start_row, end_row);

    // Gather results from all processes to process 0
    MPI_Gather(&result[start_row][0], (end_row - start_row) * n, MPI_INT, &result[0][0], (end_row - start_row) * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Process 0 prints the total execution time
    if (rank == 0)
    {
        end_time = MPI_Wtime();
        printf("%f\n", end_time - start_time);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
