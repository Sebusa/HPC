// Jacobi iteration for the Poisson equation in 1D
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

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

int main(int argc, char **argv)
{
    /* Declare variables */
    int n, nsteps; // grid size and number of steps
    double h;      // step size

    double *u, *f; // solution and RHS

    double *local_u, *local_f;  // local solution and RHS
    int local_size, size, rank; // MPI variables

    double tstart, tend; // timers

    /* Process command line arguments */
    n = (argc > 1) ? atoi(argv[1]) : 100000;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    local_size = n / size;

    /* Allocate memory for arrays */
    local_f = (double *)malloc((local_size + 1) * sizeof(double));
    local_u = (double *)malloc((local_size + 1) * sizeof(double));

    if (rank == 0)
    {
        u = (double *)malloc((n + 1) * sizeof(double));
        f = (double *)malloc((n + 1) * sizeof(double));

        /* Initialize arrays */
        h = 1.0 / n;
        memset(u, 0, (n + 1) * sizeof(double));
        for (int i = 0; i <= n; ++i)
            f[i] = i * h;
    }

    /* Distribute data */
    MPI_Scatter(f, local_size, MPI_DOUBLE, local_f, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(u, local_size, MPI_DOUBLE, local_u, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Broadcast */
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nsteps, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        tstart = MPI_Wtime(); // start timer
    }

    /* Perform Jacobi iteration */
    jacobi(nsteps, local_size, local_u, local_f);

    /* Gather data */
    MPI_Gather(local_u, local_size, MPI_DOUBLE, u, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        tend = MPI_Wtime(); // stop timer
        double cpu_time_used = tend - tstart;

        /* Print results */
        printf("%f\n", cpu_time_used);
    }

    /* Free memory allocated for arrays */
    free(local_f);
    free(local_u);
    if (rank == 0)
    {
        free(u);
        free(f);
    }

    /* Finalize MPI */
    MPI_Finalize();

    return 0;
}