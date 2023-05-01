#include "worker.h"

class Chief
{
    Worker worker;

    int full_x_size;
    int full_t_size;

    int default_size;

    int rank;
    int commsize;

    public:

    Chief (int t_size, int x_size, 
           double T, double X): full_t_size (t_size), full_x_size (x_size)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &commsize);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        default_size = full_x_size / commsize;

        int w_x_size = default_size;

        if (rank == commsize - 1)
            w_x_size = full_x_size - default_size * (commsize - 1);

        assert (w_x_size > 0);

        double tau = T / (t_size - 1);
        double h = X / (x_size - 1);

        double X_start = default_size * rank * h;
        double X_end = X_start + (w_x_size - 1) * h;

        worker = Worker (t_size, w_x_size, default_size, tau, h, T, X_start, X_end); 
    }

    int run ()
    {
        return worker.run();
    }

    int print ()
    {
        return worker.print();
    }
};