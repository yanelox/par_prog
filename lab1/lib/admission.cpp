#include <mpi.h>
#include <chrono>

using namespace std::chrono;

int main (int argc, char* argv[])
{
    MPI_Init (&argc, &argv);

    int rank, commsize;

    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto start = high_resolution_clock::now ();

    if (rank == 0)
    {
        double a;

        for (int i = 0; i < 1000000; ++i)
            MPI_Send (&a, 1, MPI_DOUBLE, 1, i, MPI_COMM_WORLD);
    }

    if (rank == 1)
    {
        double a;
        MPI_Status stat;

        for (int i = 0; i < 1000000; ++i)
            MPI_Recv (&a, 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD, &stat);
    }


    auto end = high_resolution_clock::now ();

    if (rank == 1)
        std::cout << duration_cast <milliseconds> (end - start).count() << std::endl;

    MPI_Finalize();
}