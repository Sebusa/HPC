// Matrix multiplication using processes and shared memory

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

// Functions declarations
int **create_shm(int size, key_t key);
void deallocate_shm(int **matrix);
void matrix_input(int **matrix, int size);
void brute_force(int **a, int **b, int **result, int parameters[3]);

// CMD arguments
int main(int argc, char *argv[])
{
    if (argc >= 3)
    {
        // Parameters
        int n = atoi(argv[1]);
        int P = atoi(argv[2]);

        if (n < 1 || P < 1)
        {
            printf("Invalid parameters. Please, try again.\n");
            printf("Parameters: <matrix size> <number of processes>");
            return 1;
        }
        else
        {
            srand(time(NULL));           // Random seed
            pid_t PROCESS_ID = getpid(); // Parent process ID

            // CPU clock
            struct timeval TSTART, TEND;
            double TIME;
            gettimeofday(&TSTART, NULL);

            // Matrix declaration
            int **a, **b, **result;

            // Keys
            key_t A_SHM_KEY = 0x1234;
            key_t B_SHM_KEY = 0x5678;
            key_t RESULT_SHM_KEY = 0x9ABC;

            // Shared memory initialization
            a = create_shm(n, A_SHM_KEY);
            b = create_shm(n, B_SHM_KEY);
            result = create_shm(n, RESULT_SHM_KEY);

            // Matrix input
            matrix_input(a, n);
            matrix_input(b, n);

            // Process creation
            pid_t pid = getpid(); // Actual process ID

            int parameters[3] = {n, P, 0}; // Matrix size, number of processes, process ID
            // We don't need to create a new process for the last one as we already count the parent process
            for (int i = 0; i < P - 1; i++)
            {
                if (pid != 0) // Parent process
                {
                    parameters[2] = i + 1;
                    brute_force(a, b, result, parameters);
                    pid = fork(); // Create a new child process
                }
                else if (pid == -1) // Error
                {
                    printf("Error creating the process.\n");
                    return 1;
                }
            }
            wait(NULL); // Wait for the child process to finish

            // End clock
            gettimeofday(&TEND, NULL);
            TIME = (TEND.tv_sec - TSTART.tv_sec) * 1000.0;    // sec to ms
            TIME += (TEND.tv_usec - TSTART.tv_usec) / 1000.0; // us to ms

            if (getpid() == PROCESS_ID)
            {
                printf("%.5lf\n", TIME / 1000.0);
            }

            // Deallocate memory
            deallocate_shm(a);
            deallocate_shm(b);
            deallocate_shm(result);
        }
    }
    else
    {
        printf("You lack of arguments. Please, try again.\n");
        printf("Parameters: <matrix size> <number of processes>\n");
    }
    return 0;
}

// Shared memory initialization
int **create_shm(int size, key_t key)
{
    // Allocate shared memory
    int shmid = shmget(key, size * sizeof(int *), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        printf("You lack of arguments. Please, try again.\n");
        printf("Parameters: <matrix size> <number of processes>\n");
        exit(1);
    }

    // Attach the shared memory
    int **matrix = (int **)shmat(shmid, NULL, 0);
    if (matrix == NULL)
    {
        perror("shmat");
        exit(1);
    }

    // Allocate memory for each row
    for (int i = 0; i < size; i++)
    {
        matrix[i] = (int *)malloc(size * sizeof(int));

        if (matrix[i] == NULL)
        {
            perror("malloc");
            exit(1);
        }
    }

    return matrix;
}

// Deallocate memory
void deallocate_shm(int **matrix)
{
    for(int i = 0; i < sizeof(matrix); i++)
    {
        free(matrix[i]);
    }
    shmdt(matrix);
}

// Matrix input (random)
void matrix_input(int **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = 1 + rand() % 9;
        }
    }
}

// Matrix multiplication with O(n^3) algorithm
void brute_force(int **a, int **b, int **result, int parameters[3])
{
    int n = parameters[0];
    int T = parameters[1];
    int ID = parameters[2];

    // Distribute the work
    int start, end;
    if (ID % T == 0)
    {
        start = (ID - 1) * (n / T);
        end = ID * (n / T);
    }
    else
    {
        start = (ID - 1) * (n / T);
        end = ID * (n / T) + (n % T);
    }

    // Matrix multiplication
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}