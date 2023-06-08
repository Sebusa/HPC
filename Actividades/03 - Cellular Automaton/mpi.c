#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void initialize(int *old, int size, int rank)
{
    srand(time(0) + rank); // Random seed based on rank

    for (int i = 1; i <= size; i++)
    {
        old[i] = rand() % 2; // Random bits values
    }
}

// Update the array of cells
void update(int *old, int *new, int size)
{
    old[0] = old[size];
    old[size + 1] = old[1];

    // Update the new array
    for (int i = 1; i <= size; i++)
    {
        if (old[i] == 1) // A car in cell
        {
            if (old[i + 1] == 1)
            {
                new[i] = 1;
            }
            else
            {
                new[i] = 0;
            }
        }
        else // Empty lot
        {
            if (old[i - 1] == 1)
            {
                new[i] = 1;
            }
            else
            {
                new[i] = 0;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int N = (argc > 1) ? atoi(argv[1]) : 1000;       // Number of cells
    int iterations = (argc > 2) ? atoi(argv[2]) : 5; // Number of iterations

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int localSize = N / size;
    int remainder = N % size;

    if (rank == 0)
    {
        localSize += remainder; // Add remainder to process 0
    }

    int *old = (int *)malloc((localSize + 2) * sizeof(int)); // 2 extra cells for the borders
    int *new = (int *)malloc((localSize + 2) * sizeof(int));

    initialize(old, localSize, rank);

    MPI_Barrier(MPI_COMM_WORLD); // Wait for all processes to initialize

    double start = MPI_Wtime();

    for (int iter = 1; iter <= iterations; iter++)
    {
        MPI_Status status;

        // Exchange border cells with neighboring processes
        if (rank != 0)
        {
            MPI_Send(&old[1], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&old[0], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        }

        if (rank != size - 1)
        {
            MPI_Send(&old[localSize], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&old[localSize + 1], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
        }

        update(old, new, localSize);

        MPI_Barrier(MPI_COMM_WORLD); // Wait for all processes to finish updating
        MPI_Gather(&new[1], localSize, MPI_INT, &old[1], localSize, MPI_INT, 0, MPI_COMM_WORLD); // Gather the updated local arrays into the root process
        MPI_Barrier(MPI_COMM_WORLD); // Wait for all processes to complete gathering

        if (rank == 0)
        {
            // Copy the updated array to the new array
            for (int i = 1; i <= localSize; i++)
            {
                old[i] = new[i];
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD); // Wait for all processes to complete iterations

    if (rank == 0)
    {
        double end = MPI_Wtime();
        double cpu_time_used = (end - start);
        printf("%f\n", cpu_time_used);
    }

    free(old);
    free(new);

    MPI_Finalize();

    return 0;
}
