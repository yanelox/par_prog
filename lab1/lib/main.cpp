#include "chief.h"
#include <sstream>
#include <chrono>

using namespace std::chrono;

int main (int argc, char* argv[])
{
    MPI_Init (&argc, &argv);

    int size = 1250;
    double T = 1.0;
    double X = 1.0;

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

    // if ((rank == 0 and commsize == 1) or (rank == 1))
    //     std::cout << duration_cast <milliseconds> (end - start).count() << std::endl;

    chief.print();

    if (rank == commsize - 1)
    {
        std::cout << size << " " << T << " " << X << std::endl; 
    }

    MPI_Finalize();
}