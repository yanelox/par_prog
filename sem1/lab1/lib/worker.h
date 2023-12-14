#include "scheme.h"

class Worker
{
    Scheme scheme;

    int rank;
    int commsize;
    int default_size;

    public:

    Worker () = default;

    Worker (int t_size_, int x_size_, int default_size_, 
            double tau_, double h_,
            double T_,
            double X_start_, double X_end_):
            scheme (t_size_, x_size_, tau_, h_, T_, X_start_, X_end_),
            default_size (default_size_)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &commsize);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        scheme.fill_zero_line ();

        if (rank == 0)
        {
            scheme.fill_zero_column ();
        }

        double left_ = 0.0;
        double right_ = 0.0;

        if (rank != 0)
            left_ = phi ((rank * default_size_ - 1)* h_);

        if (rank != commsize - 1)
            right_ = phi ((rank * default_size_ + x_size_) * h_);

        scheme.set_sides (left_, right_);
    }

    int send_sides (double left, double right)
    {
        MPI_Request req[2];

        if (rank != 0)
            MPI_Isend (&left, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, req);

        if (rank != commsize - 1)
            MPI_Isend (&right, 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, req + 1);

        return 0;
    }

    int get_sides (double *left, double *right)
    {
        if (rank != 0)
        {
            MPI_Request req;

            MPI_Irecv (left, 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &req);
            MPI_Wait (&req, MPI_STATUS_IGNORE);
        }

        if (rank != commsize - 1)
        {
            MPI_Request req;

            MPI_Irecv (right, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &req);
            MPI_Wait (&req, MPI_STATUS_IGNORE);
        }

        return 0;
    }

    int run ()
    {
        for (int i = 1; i < scheme.t_size; ++i)
        {
            if (rank != 0 and !(rank == commsize - 1 and scheme.x_size == 1))
                scheme.count_line_tps (i, 0, 1);
            else if (rank == commsize - 1 and scheme.x_size == 1)
                scheme.count_line_olc (i, 0, 1);
            
            double tmp_left = scheme.get_u (i, 0);

            if (rank != commsize - 1 and scheme.x_size > 1)
                scheme.count_line_tps (i, scheme.x_size - 1, scheme.x_size);

            double tmp_right = scheme.get_u (i, scheme.x_size - 1);

            send_sides (tmp_left, tmp_right);
            
            if (scheme.x_size > 2)
                scheme.count_line_tps (i, 1, scheme.x_size - 1);

            get_sides (&tmp_left, &tmp_right);

            scheme.set_sides (tmp_left, tmp_right);
        }

        if (rank == commsize - 1 and scheme.x_size > 1)
            scheme.count_column_olc (scheme.x_size - 1, 1, scheme.t_size);

        return 0;
    }

    int print ()
    {
        int a;

        if (rank != 0)
        {
            MPI_Status status;
            MPI_Recv (&a, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        }
        
        int res = scheme.print();

        if (rank != commsize - 1)
            MPI_Send (&a, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);

        return res;
    }
};