// Jacobi iteration for the Poisson equation in 1D applying threads
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

/* Global declarations */
int n, nsteps, T; // grid size, number of steps and number of threads
double *u, *f;    // solution and RHS

/* Function to perform nsweeps sweeps of Jacobi iteration on a 1D Poisson problem */
void *jacobi(void *threadarg)
{
    // Get thread ID
    int ID = *((int *)threadarg);

    /* Declare variables */
    double h = 1.0 / n;
    double h2 = h * h;
    double *utmp = (double *)malloc((n + 1) * sizeof(double));

    /* Fill boundary conditions into utmp */
    utmp[0] = u[0];
    utmp[n] = u[n];

    /* Distribution */
    int start = (ID - 1) * (nsteps / T);
    int end = (ID % T == 0) ? ID * (nsteps / T) : ID * (nsteps / T) + (nsteps % T);

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
    pthread_exit(NULL);
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
    double h;    // step size
    char *fname; // file name

    // CPU clock
    struct timeval TSTART, TEND;
    double TIME;

    /* Process command line arguments */
    n = (argc > 1) ? atoi(argv[1]) : 100;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    T = (argc > 3) ? atoi(argv[3]) : 1;
    fname = (argc > 4) ? argv[4] : NULL;
    h = 1.0 / n;

    /* Allocate memory for arrays */
    u = (double *)malloc((n + 1) * sizeof(double));
    f = (double *)malloc((n + 1) * sizeof(double));

    /* Initialize arrays */
    memset(u, 0, (n + 1) * sizeof(double));
    for (int i = 0; i <= n; ++i)
        f[i] = i * h;

    /* Threads declaration */
    pthread_t *threads;
    threads = (pthread_t *)malloc(T * sizeof(pthread_t));

    /* Perform Jacobi iteration with threads*/
    gettimeofday(&TSTART, NULL);

    for (int i = 0; i < T; i++)
    {
        int *ID = (int *)malloc(sizeof(int));
        *ID = i+1;
        pthread_create(&threads[i], NULL, jacobi, (void *)ID);
    }

    // Join threads
    for (int i = 0; i < T; i++)
    {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&TEND, NULL);
    TIME = (TEND.tv_sec - TSTART.tv_sec) * 1000.0;    // sec to ms
    TIME += (TEND.tv_usec - TSTART.tv_usec) / 1000.0; // us to ms

    /* Print results */
    printf("%.5lf\n", TIME / 1000.0);

    /* Write solution to file */
    if (fname)
        write_solution(n, u, fname);

    /* Free memory allocated for arrays */
    free(threads);
    free(f);
    free(u);

    return 0;
}