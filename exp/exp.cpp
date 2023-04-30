#include <mpi.h>
#include <iostream>
#include <assert.h>

int main (int argc, char* argv[])
{
    assert (argc == 2);

    MPI_Init (&argc, &argv);

    int commsize, rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    MPI_Finalize();
}