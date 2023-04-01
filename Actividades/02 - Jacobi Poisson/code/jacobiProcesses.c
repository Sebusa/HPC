// Jacobi iteration for the Poisson equation in 1D using fork() and shared memory
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

/* Create a shared memory segment using mmap() */
double *create_shm(int size, char *name)
{
    int fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("shm_open");
        exit(1);
    }

    /* Set the size of the shared memory segment */
    if (ftruncate(fd, size * sizeof(double)) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    /* Map the shared memory segment into the process address space */
    double *array = (double *)mmap(NULL, size * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (array == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    close(fd);
    return array;
}

/* Deallocate and 'destroy' the shared memory segment */
void destroy_shm(double *segment, char *name, int size)
{
    munmap(segment, size * sizeof(double));
    shm_unlink(name);
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
    int n, nsteps, P;            // grid size, number of steps and number of Procesess
    double h;                    // step size
    char *fname;                 // file name
    pid_t PROCESS_ID = getpid(); // Main process ID

    // Shared memory
    double *u, *f;           // solution and RHS
    int shmid_u, shmid_f;    // shared memory ID
    char *U_NAME = "/u_shm"; // Name of shared memory segments
    char *F_NAME = "/f_shm";

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
    u = create_shm(n + 1, U_NAME);
    f = create_shm(n + 1, F_NAME);

    /* Initialize arrays */
    memset(u, 0, (n + 1) * sizeof(double));
    for (int i = 0; i <= n; ++i)
        f[i] = i * h;

    /* Perform Jacobi iteration */
    gettimeofday(&tstart, NULL);
    pid_t pid = getpid(); // Actual process

    for (int i = 1; i < P; i++)
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
        printf("%5lf\n", TIME / 1000.0);

        /* Write solution to file */
        if (fname)
            write_solution(n, u, fname);

        /* Deattach the shared memory segment */
        destroy_shm(u, U_NAME, n + 1);
        destroy_shm(f, F_NAME, n + 1);
    }

    return 0;
}