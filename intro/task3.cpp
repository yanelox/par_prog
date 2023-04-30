#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <iostream>

#define TAG 223

int main (int argc, char* argv[])
{
    MPI_Init (&argc, &argv);

    int commsize, rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int res = 42;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(1,10);
    int r = (int) dist6 (rng);

    if (rank == 0)
    {
        MPI_Send (&res, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);

        MPI_Recv (&res, 1, MPI_INT, commsize - 1, TAG, MPI_COMM_WORLD, NULL);

        std::cout << "rank 0; send 42, get " << res << "\n"; 
    }

    else
    {
        MPI_Recv (&res, 1, MPI_INT, rank - 1, TAG, MPI_COMM_WORLD, NULL);

        std::cout << "rank " << rank << "; get " << res << ", send " << res + r << std::endl;

        res += r;

        if (rank == commsize - 1)
            MPI_Send (&res, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
        
        else
            MPI_Send (&res, 1, MPI_INT, rank + 1, TAG, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
}