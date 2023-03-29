// Jacobi iteration for the Poisson equation in 1D
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Function to perform nsweeps sweeps of Jacobi iteration on a 1D Poisson problem */
void jacobi(int nsweeps, int n, double *u, double *f)
{
    /* Declare variables */
    int i, sweep;
    double h = 1.0 / n;
    double h2 = h * h;
    double *utmp = (double *)malloc((n + 1) * sizeof(double));

    /* Fill boundary conditions into utmp */
    utmp[0] = u[0];
    utmp[n] = u[n];

    /* Perform Jacobi iteration */
    for (sweep = 0; sweep < nsweeps; sweep += 2)
    {
        /* Old data in u; new data in utmp */
        for (i = 1; i < n; ++i)
            utmp[i] = (u[i - 1] + u[i + 1] + h2 * f[i]) / 2;

        /* Old data in utmp; new data in u */
        for (i = 1; i < n; ++i)
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
    int n, nsteps;        // grid size and number of steps
    double *u, *f;        // solution and RHS
    double h;             // step size
    char *fname;          // file name
    clock_t tstart, tend; // timers

    /* Process command line arguments */
    n = (argc > 1) ? atoi(argv[1]) : 100;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    fname = (argc > 3) ? argv[3] : NULL;
    h = 1.0 / n;

    /* Allocate memory for arrays */
    u = (double *)malloc((n + 1) * sizeof(double));
    f = (double *)malloc((n + 1) * sizeof(double));

    /* Initialize arrays */
    memset(u, 0, (n + 1) * sizeof(double));
    for (int i = 0; i <= n; ++i)
        f[i] = i * h;

    /* Perform Jacobi iteration */
    tstart = clock();
    jacobi(nsteps, n, u, f);
    tend = clock();

    double cpu_time_used = ((double)(tend - tstart)) / CLOCKS_PER_SEC;

    /* Print results */
    printf("%f\n", cpu_time_used);

    /* Write solution to file */
    if (fname)
        write_solution(n, u, fname);

    /* Free memory allocated for arrays */
    free(f);
    free(u);
    return 0;
}