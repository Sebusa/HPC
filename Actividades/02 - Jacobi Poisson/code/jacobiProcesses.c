// Jacobi iteration for the Poisson equation in 1D using fork() and shared memory
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

/* Create a shared memory segment */
double *create_shm(int size, key_t key, int shmid)
{
    // Create the segment
    shmid = shmget(key, size * sizeof(double), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(1);
    }

    // Attach the segment
    double *array = (double *)shmat(shmid, NULL, 0);
    if (array ==  NULL)
    {
        perror("shmat");
        exit(1);
    }

    return array;
}

/* Deallocate and 'destroy' the shared memory segment */
void destroy_shm(double *segment, int shmid)
{
    shmdt(segment);
    shmctl(shmid, IPC_RMID, 0);
}

/* Function to perform nsweeps sweeps of Jacobi iteration on a 1D Poisson problem */
void jacobi(int nsweeps, int n, double *u, double *f, int pid, int P)
{
    /* Declare variables */
    double h = 1.0 / n;
    double h2 = h * h;
    double *utmp = (double *)malloc((n + 1) * sizeof(double));

    /* Fill boundary conditions into utmp */
    utmp[0] = u[0];
    utmp[n] = u[n];

    /* Distribution */
    int start = (pid - 1) * (nsweeps / P);
    int end = (pid % P == 0) ? pid * (nsweeps / P) : pid * (nsweeps / P) + (nsweeps % P);

    /* Perform Jacobi iteration */
    for (int sweep = start; sweep < end; sweep += 2)
    {
        /* Old data in u; new data in utmp */
        for (int i = 1; i < n; ++i)
            utmp[i] = (u[i - 1] + u[i + 1] + h2 * f[i]) / 2;

        /* Old data in utmp; new data in u */
        for (int i = 1; i < n; ++i)
            u[i] = (utmp[i - 1] + utmp[i + 1] + h2 * f[i]) / 2;
    }

    /* Free memory allocated to utmp */
    free(utmp);
}

/* Function to write the solution to a file */
void write_solution(int n, double *u, const char *fname)
{
    /* Declare variables */
    int i;
    double h = 1.0 / n;
    FILE *fp = fopen(fname, "w+");

    /* Write solution to file */
    for (i = 0; i <= n; ++i)
        fprintf(fp, "%g %g\n", i * h, u[i]);

    /* Close file */
    fclose(fp);
}

int main(int argc, char **argv)
{
    /* Declare variables */
    int n, nsteps, P;        // grid size, number of steps and number of Procesess
    double h;             // step size
    char *fname;          // file name
    pid_t PROCESS_ID = getpid(); //Main process ID

    // Shared memory
    double *u, *f;        // solution and RHS
    int shmid_u, shmid_f; // shared memory ID
    key_t U_KEY = 0x1234; // Keys
    key_t F_KEY = 0x5678;

    // Timers
    struct timeval tstart, tend;
    double TIME;

    /* Process command line arguments */
    n = (argc > 1) ? atoi(argv[1]) : 100;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    P = (argc > 3) ? atoi(argv[3]) : 1;
    fname = (argc > 4) ? argv[4] : NULL;
    h = 1.0 / n;

    /* Allocate memory for arrays */
    u = create_shm(n+1, U_KEY, shmid_u);
    f = create_shm(n+1, F_KEY, shmid_f);

    /* Initialize arrays */
    memset(u, 0, (n + 1) * sizeof(double));
    for (int i = 0; i <= n; ++i)
        f[i] = i * h;

    /* Perform Jacobi iteration */
    gettimeofday(&tstart, NULL);
    pid_t pid = getpid(); // Actual process

    for(int i = 1; i < P; i++)
    {
        if (pid != 0)
        {
            jacobi(nsteps, n, u, f, i, P);
            pid = fork(); // New child
        }
        else if (pid == -1)
        {
            perror("pid");
            return 1;
        } 
    }
    wait(NULL); // Wait for the child process

    gettimeofday(&tend, NULL);
    TIME = (tend.tv_sec - tstart.tv_sec) * 1000.0;    // sec to ms
    TIME += (tend.tv_usec - tstart.tv_usec) / 1000.0; // us to ms

    if (getpid() == PROCESS_ID)
    {
        /* Print results */
        printf("n: %d\n"
               "nsteps: %d\n"
               "Elapsed time: %5lf s\n",
               n, nsteps, TIME/1000.0);

        /* Write solution to file */
        if (fname)
            write_solution(n, u, fname);

        /* Deattach the shared memory segment */
        destroy_shm(u, shmid_u);
        destroy_shm(f, shmid_f);
    }

    return 0;
}