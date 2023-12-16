#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define ISIZE 1000
#define JSIZE 1000
// 1Д
// запускать на 4 или 8 потоках

int main(int argc, char **argv)
{
    assert(argc == 2);

    char* filename = argv[1];

    MPI_Init (&argc, &argv);

    int commsize, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double a[ISIZE][JSIZE];
    int i, j;

    FILE *ff;

    for (i = 0; i < ISIZE; i++)
        for (j = 0; j < JSIZE; j++)
            a[i][j] = 10 * i + j;

    //начало измерения времени
    double time_start = MPI_Wtime();
    int op_count = 0;
    for (i = 8 + rank; i < ISIZE; i += commsize)
        for (j = 0; j < JSIZE - 3; j++)
        {
            a[i][j] = sin(4 * a[i - 8][j + 3]);
            op_count++;
        }
    //окончание измерения времени
    double time_end = MPI_Wtime();

    printf ("%d: time=%f, count=%d\n", rank, time_end - time_start, op_count);

    if (rank != 0)
    {
        for (i = 8 + rank; i < ISIZE; i += commsize)
            for (j = 0; j < JSIZE - 3; j++)
                MPI_Send (&(a[i][j]), 1, MPI_DOUBLE, 0, i * JSIZE + j, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Status status;
        for (i = 8 + 1; i < ISIZE; i++)
        {
            if ((i - 8) % commsize == 0)
                continue;

            for (j = 0; j < JSIZE - 3; j++)
            {
                int source = (i - 8) % commsize;

                MPI_Recv (&(a[i][j]), 1, MPI_DOUBLE, source, i * JSIZE + j, MPI_COMM_WORLD, &status);
            }
        }
    }

    if (rank == 0)
    {
        ff = fopen(filename,"w");

        for(i = 0; i < ISIZE; i++)
            for (j = 0; j < JSIZE; j++)
                fprintf(ff,"%d %d %f\n", i, j, a[i][j]);

        fclose(ff);
    }

    MPI_Finalize();
}
