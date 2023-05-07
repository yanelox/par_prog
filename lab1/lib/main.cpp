#include "chief.h"
#include <sstream>
#include <chrono>

using namespace std::chrono;

int main (int argc, char* argv[])
{
    MPI_Init (&argc, &argv);

    int size = 1250;

    if (argc > 1)
    {
        std::istringstream input{argv[1]};

        input >> size;
    }

    Chief chief(size, size, 1.0, 1.0);

    auto start = high_resolution_clock::now ();
    
    chief.run();

    auto end = high_resolution_clock::now ();

    int rank, commsize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);

    if ((rank == 0 and commsize == 1) or (rank == 1))
        std::cout << duration_cast <milliseconds> (end - start).count() << std::endl;

    // chief.print();

    MPI_Finalize();
}