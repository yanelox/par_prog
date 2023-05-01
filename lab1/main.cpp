#include "chief.h"
#include <chrono>

using namespace std::chrono;

int main (int argc, char* argv[])
{
    MPI_Init (&argc, &argv);

    int size = 10000;

    Chief chief(size, size, 1.0, 1.0);

    auto start = high_resolution_clock::now ();
    
    chief.run();

    auto end = high_resolution_clock::now ();

    std::cout << duration_cast <milliseconds> (end - start).count() << std::endl;

    // chief.print();

    MPI_Finalize();
}