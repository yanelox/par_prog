#include "chief.h"

int randomize (std::vector <int> &dest)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 1);

    for (auto& i:dest)
        i = (int) dist6 (rng);

    return 0;
}

using namespace std::chrono;

int main (int argc, char* argv[])
{
    MPI_Init (&argc, &argv);

    int commsize, rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int size = 500;

    std::vector <int> init (size * size);
    randomize (init);

    chief w (size, size, rank, commsize, init);

    auto start = high_resolution_clock::now ();

    for (int i = 0; i < 100; ++i)
        w.run_once();
    
    auto end = high_resolution_clock::now ();

    std::cout << duration_cast <milliseconds> (end - start).count() / 100 << std::endl;

    // w.print();

    // worker main_w (12, 5, rank, commsize, init);
    // main_w.run_once();

    // const std::vector <int> &res = main_w.get_stash();

    // for (int i = 0; i < 5; ++i)
    // {
    //     for (int j = 0; j < 12; ++j)
    //         std::cout << res[i * 12 + j] << " ";

    //     std::cout << std::endl;
    // }

    MPI_Finalize();
}