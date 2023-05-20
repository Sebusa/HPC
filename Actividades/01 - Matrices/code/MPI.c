#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

// Functions declarations
int **initialize(int n, int section) // Allocate memory for a matrix
{
    int **matrix = (int **)malloc(n * sizeof(int *));

    int row = (section == 0) ? n : section;
    for (int i = 0; i < row; i++)
    {
        matrix[i] = (int *)malloc(n * sizeof(int));
    }
    return matrix;
}

int **deallocate(int **matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
    return matrix;
}

void input(int **matrix, int n) // Random input
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

void bruteForce(int **localA, int **localB, int **localResult, int startRow, int endRow, int n)
{
    for (int i = startRow; i < endRow; i++)
    {
        for (int j = 0; j < n; j++)
        {
            localResult[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                localResult[i][j] += localA[i][k] * localB[k][j];
            }
        }
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL)); // Random seed

    int rank, size;
    MPI_Init(&argc, &argv);               // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtain the rank of the calling process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtain the total number of processes

    int n = (argc > 1) ? atoi(argv[1]) : 1000; // Matrix size (default 1000)
    int **A, **B, **result;

    double start_time, end_time; // Time variables

    if (rank == 0) // Process 0 initializes the matrices
    {
        A = initialize(n, 0);
        B = initialize(n, 0);
        result = initialize(n, 0);

        input(A, n);
        input(B, n);

        start_time = MPI_Wtime(); // Start time
    }

    // Communicate the matrix size to all the processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /*
    Allocate memory for the local matrices
    */
    int rowsPerProcess = ((n % size) == 0) ? n / size : (n / size) + 1;

    int **localA = initialize(n, rowsPerProcess);
    int **localB = initialize(n, 0);
    int **localResult = initialize(n, rowsPerProcess);

    /*
    Distribute the matrices to all the processes
    */

    //Ni puta idea qué hace esto jsjs
    MPI_Scatter(A, rowsPerProcess * n, MPI_INT, localA[0], rowsPerProcess * n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B[0], n * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Local matrix multiplication
    bruteForce(localA, localB, localResult, 0, rowsPerProcess, n);

    // Gather the results from all the processes to the process 0
    MPI_Gather(localResult[0], rowsPerProcess * n, MPI_INT, result[0], rowsPerProcess * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Final process
    if (rank == 0)
    {
        end_time = MPI_Wtime(); // End time
        printf("%f\n", end_time - start_time);

        // Free memory
        deallocate(A, n);
        deallocate(B, n);
        deallocate(result, n);
    }

    MPI_Finalize();
    return 0;
}
