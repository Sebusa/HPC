#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initialize(int old[], int size)
{
    for (int i = 1; i <= size; i++)
    {
        old[i] = rand() % 2; // Random bits values
    }
}

// Update the array of cells
void update(int old[], int new[], int size, int *totalMoves)
{
    old[0] = old[size];
    old[size + 1] = old[1];
    *totalMoves = 0; // Initialize totalMoves

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
                (*totalMoves)++;
            }
        }
        else // Empty lot
        {
            if (old[i - 1] == 1)
            {
                new[i] = 1;
                (*totalMoves)++;
            }
            else
            {
                new[i] = 0;
            }
        }
    }

    // Copy the new array to the old array
    for (int i = 1; i <= size; i++)
    {
        old[i] = new[i];
    }
}

int main(int argc, char *argv[])
{
    int N = (argc > 1) ? atoi(argv[1]) : 1000;       // Number of cells
    int iterations = (argc > 2) ? atoi(argv[2]) : 5; // Number of iterations

    int *old = malloc((N + 2) * sizeof(int)); // 2 extra positions
    int *new = malloc((N + 2) * sizeof(int));

    // CPU clock
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    srand(time(0)); // Random Input Seed

    initialize(old, N);

    for (int i = 1; i <= iterations; i++)
    {
        int totalMoves = 0; // Total number of cells that move

        update(old, new, N, &totalMoves);

        double averageSpeed = (double)totalMoves / N;
    }

    // End clock
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("%f\n", cpu_time_used);

    free(old);
    free(new);

    return 0;
}
