#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void bruteForce(int *A, int *B, int *result, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            result[i * n + j] = 0;
            for (int k = 0; k < n; k++)
            {
                result[i * n + j] += A[i * n + k] * B[k * n + j];
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

    int *a, *b, *result;         // Input matrices and result matrix
    int *local_a, *local_result; // Local matrices

    int n = (argc > 1) ? atoi(argv[1]) : 1000; // Matrix size
    int size, rank;                            // MPI variables

    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("Inicio\n");

    int local_size = n / size;

    // Allocate memory for local matrices
    local_a = initialize(local_size, n);
    local_result = initialize(local_size, n);

    // Only rank 0 will have the full matrices a and b
    if (rank == 0)
    {
        a = initialize(n, n);
        b = initialize(n, n);
        result = initialize(n, n);

        // Initialize matrices a and b with random values
        input(a, n);
        input(b, n);
    }
    printf("matrices\n");

    // Scatter matrix a
    MPI_Scatter(a, local_size * n, MPI_INT, local_a, local_size * n, MPI_INT, 0, MPI_COMM_WORLD);
    printf("A\n");

    // Broadcast matrix b
    MPI_Bcast(b, n * n, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Cast\n");

    // Start timing
    if (rank == 0)
    {
        start_time = MPI_Wtime();
        printf("Tiempo\n");
    }

    // Perform matrix multiplication
    bruteForce(local_a, b, local_result, local_size);
    printf("Algoritmo\n");

    // Gather results
    MPI_Gather(local_result, local_size * n, MPI_INT, result, local_size * n, MPI_INT, 0, MPI_COMM_WORLD);
    printf("resultados\n");

    // End timing
    if (rank == 0)
    {
        end_time = MPI_Wtime();
        double total_time = end_time - start_time;
        printf("%f\n", total_time);
    }

    // Free memory
    free(local_a);
    free(local_result);
    if (rank == 0)
    {
        free(a);
        free(b);
        free(result);
    }
    printf("Fin\n");

    MPI_Finalize(); // Finalize MPI

    return 0;
}
