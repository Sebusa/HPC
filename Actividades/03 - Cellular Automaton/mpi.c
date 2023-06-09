#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void initialize(int old[], int size)
{
    for (int i = 1; i <= size; i++)
    {
        old[i] = rand() % 2;
    }
}

void update(int old[], int new[], int size, int rank)
{
    old[0] = old[size];
    old[size + 1] = old[1];

    // Actualiza el nuevo arreglo
    for (int i = 1; i <= size; i++)
    {
        if (old[i] == 1)
        { // Celda ocupada
            if (old[i + 1] == 1)
            {
                new[i] = 1;
            }
            else
            {
                new[i] = 0;
            }
        }
        else
        { // Arreglo vacío
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

    double start, end, total_time;

    MPI_Init(&argc, &argv);

    int chunk, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &chunk);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(0)); //Random seed

    int local_size = N / chunk;
    int remainder = N % chunk; // Remainder of N / chunk

    int *old, *new;

    local_size += (rank == 0 && remainder != 0)? remainder : 0; // Add remainder to head node
    old = (int *)malloc((local_size + 2) * sizeof(int));
    new = (int *)malloc((local_size + 2) * sizeof(int));

    initialize(old, local_size);

    if(rank == 0){
        start = MPI_Wtime();
    }

    MPI_Barrier(MPI_COMM_WORLD); // Wait for all nodes to start

    for (int i = 1; i <= iterations; i++)
    {

        update(old, new, local_size, rank);

        // Communicate boundary cells
        if (chunk > 1)
        {
            if (rank % 2 == 0)
            {
                if (rank < chunk - 1)
                {
                    MPI_Send(&new[local_size], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&new[local_size + 1], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
                else if (rank > 0)
                {
                    MPI_Send(&new[1], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&new[0], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }
            else
            {
                MPI_Recv(&new[0], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&new[1], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                if (rank < chunk - 1)
                {
                    MPI_Recv(&new[local_size + 1], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Send(&new[local_size], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
            }
        }

        // Copy new to old
        for (int i = 1; i <= local_size; i++)
        {
            old[i] = new[i];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD); // Wait for all nodes to finish

    free(old);
    free(new);

    if (rank == 0)
    {
        end = MPI_Wtime();
        total_time = end - start;
        printf("%f\n", total_time);
    }

    MPI_Finalize();

    return 0;
}