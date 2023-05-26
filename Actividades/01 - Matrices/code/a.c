#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MATRIX_SIZE 1000

void matrix_multiply(int *A, int *B, int *C, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            C[i * size + j] = 0;
            for (int k = 0; k < size; k++)
            {
                C[i * size + j] += A[i * size + k] * B[k * size + j];
            }
        }
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    int *A, *B, *C;
    int *local_A, *local_C;
    int local_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_size = MATRIX_SIZE / size;

    // Allocate memory for local matrices
    local_A = (int *)malloc(local_size * MATRIX_SIZE * sizeof(int));
    local_C = (int *)malloc(local_size * MATRIX_SIZE * sizeof(int));

    // Only rank 0 will have the full matrices A and B
    if (rank == 0)
    {
        A = (int *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
        B = (int *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
        C = (int *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));

        // Initialize matrices A and B with random values
        for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++)
        {
            A[i] = rand() % 10;
            B[i] = rand() % 10;
        }
    }

    // Scatter the matrix A to all processes
    MPI_Scatter(A, local_size * MATRIX_SIZE, MPI_INT,
                local_A, local_size * MATRIX_SIZE, MPI_INT,
                0, MPI_COMM_WORLD);

    // Broadcast matrix B to all processes
    MPI_Bcast(B, MATRIX_SIZE * MATRIX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform local matrix multiplication
    matrix_multiply(local_A, B, local_C, local_size);

    // Gather the results from all processes to rank 0
    MPI_Gather(local_C, local_size * MATRIX_SIZE, MPI_INT,
               C, local_size * MATRIX_SIZE, MPI_INT,
               0, MPI_COMM_WORLD);

    // Print the result matrix (only by rank 0)
    if (rank == 0)
    {
        printf("Result Matrix:\n");
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                printf("%d ", C[i * MATRIX_SIZE + j]);
            }
            printf("\n");
        }
    }

    // Free memory
    free(local_A);
    free(local_C);
    if (rank == 0)
    {
        free(A);
        free(B);
        free(C);
    }

    MPI_Finalize();

    return 0;
}
