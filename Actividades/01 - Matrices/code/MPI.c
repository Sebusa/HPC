#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void bruteForce(int *A, int *B, int *result, int size, int n)
{

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < n; j++)
        {
            result[i * size + j] = 0;
            for (int k = 0; k < n; k++)
            {
                result[i * size + j] += A[i * size + k] * B[k * n + j];
            }
        }
    }
}

int *initialize(int x, int y)
{
    int *matrix;
    matrix = (int *)malloc(x * y * sizeof(int *));
    return matrix;
}

void input(int *matrix, int n)
{
    // Random input
    for (int i = 0; i < n * n; i++)
    {
        matrix[i] = 1 + rand() % 9;
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL)); // Random seed

    double start_time, end_time; // Timing variables

    int *a, *b, *result;                       // Input matrices and result matrix
    int n = (argc > 1) ? atoi(argv[1]) : 1000; // Matrix size
    int size, rank, local_size;                // MPI variables

    // Initialize matrices
    a = initialize(n, n);
    b = initialize(n, n);
    result = initialize(n, n);

    // Initialize matrices a and b with random values
    input(a, n);
    input(b, n);

    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Local size of each process (except head process)
    local_size = n / size;

    // Initialize local matrices
    int *local_a = initialize(local_size, n);
    int *local_result = initialize(local_size, n);

    // Head process
    if (rank == 0)
    {
        start_time = MPI_Wtime(); // Start timer
    }

    // Scatter matrix A to each process
    MPI_Scatter(a, local_size * n, MPI_INT, local_a, local_size * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcast matrix B and local_size to each process
    MPI_Bcast(b, n * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Matrix multiplication
    printf("Antes\n");
    printf("%d %d\n", local_size, n);
    bruteForce(local_a, b, local_result, local_size, n);
    printf("Algoritmo %d\n", rank);

    // Gather result from each process
    MPI_Gather(local_result, local_size * n, MPI_INT, result, local_size * n, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Gather %d\n", rank);

    // Free local matrices
    free(local_a);
    free(local_result);

    // End timing
    if (rank == 0)
    {
        end_time = MPI_Wtime();
        double total_time = end_time - start_time;
        printf("%f\n", total_time);

        // Free matrices
        free(a);
        free(b);
        free(result);
    }

    MPI_Finalize(); // Finalize MPI
    return 0;
}
