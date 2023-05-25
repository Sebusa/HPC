// Jacobi iteration for the Poisson equation in 1D with MPI
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

/* Function to perform nsweeps sweeps of Jacobi iteration on a 1D Poisson problem */
void jacobi(int nsweeps, int n, double *u, double *f, int rank, int size)
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

        // Communication between processes
        if (rank > 0)
            MPI_Send(&utmp[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        if (rank < size - 1)
            MPI_Send(&utmp[n - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);

        if (rank > 0)
            MPI_Recv(&utmp[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (rank < size - 1)
            MPI_Recv(&utmp[n], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

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
    int n, nsteps;       // grid size and number of steps
    double *u, *f;       // solution and RHS
    double h;            // step size
    int rank, size;      // MPI variables
    char *fname;         // file name
    double tstart, tend; // timers

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Process command line arguments */
    n = (argc > 1) ? atoi(argv[1]) : 1000;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    h = 1.0 / n;

    /* Calculate local size */
    int local_n = n / size;
    int local_start = rank * local_n;
    int local_end = local_start + local_n - 1;

    if (rank == 0)
    {
        /* Allocate memory for local arrays */
        u = (double *)malloc((local_n + 1) * sizeof(double));
        f = (double *)malloc((local_n + 1) * sizeof(double));

        /* Initialize local arrays */
        memset(u, 0, (local_n + 1) * sizeof(double));
        for (int i = 0; i <= local_n; ++i)
            f[i] = (i + local_start) * h;

        tstart = MPI_Wtime();
    }

    /* Perform Jacobi iteration */
    jacobi(nsteps, local_n, u, f, rank, size);

    /* Gather results from all processes */
    double *global_u = NULL;
    if (rank == 0)
    {
        global_u = (double *)malloc((n + 1) * sizeof(double));
    }

    MPI_Gather(u, local_n, MPI_DOUBLE, global_u, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Print results */
    if (rank == 0)
    {
        tend = MPI_Wtime();
        printf("%f\n", tend - tstart);

        /* Free memory allocated for arrays */
        free(global_u);

        /* Free memory allocated for local arrays */
        free(f);
        free(u);
    }

    /* Finalize MPI */
    MPI_Finalize();

    return 0;
}
