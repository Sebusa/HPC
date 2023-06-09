// Purpose: Serial implementation of the Game of Life (Cellular Automaton)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CACHE_LINE_SIZE 8 // Caché line size in bytes

void initialize(char old[], int size)
{
    for (int i = 0; i < (size + 2) / CACHE_LINE_SIZE; i++)
    {
        old[i] = rand() & 0xFF; // Random 8-bit values
    }
}

// Update the array of cells
void update(char old[], char new[], int size)
{
    for (int i = 0; i < (size + 2) / CACHE_LINE_SIZE; i++)
    {
        char current[CACHE_LINE_SIZE];
        char left[CACHE_LINE_SIZE];
        char right[CACHE_LINE_SIZE];

        // Get the current, left and right cells
        for (int j = 0; j < CACHE_LINE_SIZE; j++)
        {
            current[j] = (old[i * CACHE_LINE_SIZE + j / 8] >> (j % 8)) & 0x01;
            left[j] = (old[((i - 1 + (size + 2) / CACHE_LINE_SIZE) % ((size + 2) / CACHE_LINE_SIZE)) * CACHE_LINE_SIZE + j / 8] >> (j % 8)) & 0x01;
            right[j] = (old[((i + 1) % ((size + 2) / CACHE_LINE_SIZE)) * CACHE_LINE_SIZE + j / 8] >> (j % 8)) & 0x01;
        }

        // Update the new array
        for (int j = 0; j < CACHE_LINE_SIZE; j++)
        {
            char bit = current[j];
            char leftBit = left[j];
            char rightBit = right[j];

            if ((i * CACHE_LINE_SIZE + j) >= 1 && (i * CACHE_LINE_SIZE + j) <= size) // Ignore the borders
            {
                if (bit == 1) // Alive cell
                {
                    if (rightBit == 1) // Right neighbor is alive
                    {
                        new[i * CACHE_LINE_SIZE + j / 8] |= (0x01 << (j % 8));
                    }
                    else
                    {
                        new[i * CACHE_LINE_SIZE + j / 8] &= ~(0x01 << (j % 8));
                    }
                }
                else // Dead cell
                {
                    if (leftBit == 1) // Left neighbor is alive
                    {
                        new[i * CACHE_LINE_SIZE + j / 8] |= (0x01 << (j % 8));
                    }
                    else
                    {
                        new[i * CACHE_LINE_SIZE + j / 8] &= ~(0x01 << (j % 8));
                    }
                }
            }
        }
    }

    // Copy the new array to the old array
    for (int i = 0; i < (size + 2) / CACHE_LINE_SIZE; i++)
    {
        old[i] = new[i];
    }
}

int main(int argc, char *argv[])
{
    int N = (argc > 1) ? atoi(argv[1]) : 1000;       // Number of cells
    int iterations = (argc > 2) ? atoi(argv[2]) : 5; // Number of iterations

    int byteSize = (N + 2) / 8; // Size in bytes
    int cacheLineCount = (byteSize + CACHE_LINE_SIZE - 1) / CACHE_LINE_SIZE; // Number of caché lines

    char *old = (char *)malloc(cacheLineCount * CACHE_LINE_SIZE);
    char *new = (char *)malloc(cacheLineCount * CACHE_LINE_SIZE);

    // CPU clock
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    srand(time(0)); // Random Input Seed

    initialize(old, N);

    for (int i = 1; i <= iterations; i++)
    {
        update(old, new, N);
    }

    // End clock
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("%f\n", cpu_time_used);

    free(old);
    free(new);

    return 0;
}
