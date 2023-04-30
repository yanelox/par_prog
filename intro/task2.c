#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main (int argc, char* argv[])
{
    assert (argc == 2);

    MPI_Init (&argc, &argv);

    int n = atoi (argv[1]);
    int commsize, rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int count = n / commsize + ((n % commsize != 0) ? 1 : 0);

    if (n < commsize)
        count = 1;

    float sum = 0.0;

    for (int i = count * rank; i < count * (rank + 1) && i < n; ++i)
        sum += 1.0 / (i + 1);

    if (rank != 0)
        MPI_Send (&sum, 1, MPI_FLOAT, 0, 223, MPI_COMM_WORLD);

    else
    {
        float tmp = 0.0;
        MPI_Status status;

        for (int source = 1; source < commsize; ++source)
        {
            MPI_Recv (&tmp, 1, MPI_FLOAT, source, 223, MPI_COMM_WORLD, &status);

            sum += tmp;
        }

        printf ("sum = %.5f\n", sum);
    }
    
    MPI_Finalize();
}