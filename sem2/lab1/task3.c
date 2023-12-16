#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define ISIZE 1000
#define JSIZE 1000

double a[ISIZE][JSIZE];
double b[ISIZE][JSIZE];

// 3В

int main(int argc, char **argv)
{
    assert(argc == 2);

    char* filename = argv[1];

    MPI_Init (&argc, &argv);

    int commsize, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int i, j;
    FILE *ff;

    for (i = 0; i < ISIZE; i++)
        for (j = 0; j < JSIZE; j++)
        {
            a[i][j] = 10 * i + j;
            b[i][j] = 0;
        }
    //начало измерения времени
    double time_start = MPI_Wtime();
    int sin_op_count = 0;
    int b_op_count = 0;

    for (i = rank; i < ISIZE; i += commsize)
        for (j = 0; j < JSIZE; j++)
        {
            a[i][j] = sin (0.01 * a[i][j]);
            sin_op_count++;
        }

    for (i = rank; i < ISIZE; i += commsize)
        for (j = 2; j < JSIZE; j++)
        {
            b[i][j] = a[i][j - 2] * 2.5;
            b_op_count++;
        }
    //окончание измерения времени
    double time_end = MPI_Wtime();

    printf ("%d: time=%f, sin_count=%d, b_count=%d\n", rank, time_end - time_start, sin_op_count, b_op_count);

    if (rank != 0)
    {
        for (i = rank; i < ISIZE; i += commsize)
            for (j = 2; j < JSIZE; j++)
                MPI_Send (&(b[i][j]), 1, MPI_DOUBLE, 0, i * JSIZE + j, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Status status;
        for (i = 1; i < ISIZE; i++)
        {
            if (i % commsize == 0)
                continue;

            for (j = 2; j < JSIZE; j++)
            {
                int source = i % commsize;
                MPI_Recv (&(b[i][j]), 1, MPI_DOUBLE, source, i * JSIZE + j, MPI_COMM_WORLD, &status);
            }
        }
    }

    if (rank == 0)
    {
        ff = fopen(filename,"w");

        for(i = 0; i < ISIZE; i++)
            for (j = 0; j < JSIZE; j++)
                fprintf(ff,"%d %d %f\n", i, j, b[i][j]);

        fclose(ff);
    }

    MPI_Finalize();
}
