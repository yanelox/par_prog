#include "field.h"

class worker
{
    field internal_field;
    
    int rank;
    int commsize;

    int width;
    int height;
    
    std::vector <int> up;
    std::vector <int> down;

    std::vector <int> str1;
    std::vector <int> str2;

    void requset_updown ()
    {
        MPI_Request req[4];

        int up_number = (rank - 1 == -1) ? commsize - 1 : rank - 1; 
        int down_number = (rank + 1 == commsize) ? 0 : rank + 1;

        internal_field.get_str (str1.begin(), 0);
        internal_field.get_str (str2.begin(), internal_field.get_h() - 1);

        // sending to upper worker
        MPI_Isend (str1.data(), width, MPI_INT, up_number, 0, MPI_COMM_WORLD, req);
        // sending to down worker
        MPI_Isend (str2.data(), width, MPI_INT, down_number, 1, MPI_COMM_WORLD, req + 1);

        // getting from upper worker
        MPI_Irecv (up.data(), width, MPI_INT, up_number, 1, MPI_COMM_WORLD, req + 2);
        // getting from down worker
        MPI_Irecv (down.data(), width, MPI_INT, down_number, 0, MPI_COMM_WORLD, req + 3);

        for (auto& i:req)
            MPI_Wait (&i, MPI_STATUS_IGNORE);
    }

    public:

    worker () = default;

    worker (int width_, int height_, 
            int rank_, int commsize_): width (width_), height (height_),
                                      internal_field(width_, height_), 
                                      rank(rank_), commsize(commsize_),
                                      up (width_), down (width_),
                                      str1 (width_), str2 (width_) 
    {
    }

    worker (int width_, int height_, 
            int rank_, int commsize_,
            std::vector <int> &init): width (width_), height (height_),
                                      internal_field(width_, height_, init), 
                                      rank(rank_), commsize(commsize_),
                                      up (width_), down (width_),
                                      str1 (width_), str2 (width_) 
    {
    }

    template <class InputIt>
    int init_field (InputIt source)
    {
        return internal_field.init_field (source);
    }

    int run_once ()
    {
        requset_updown();
        internal_field.update_updown (up, down);
        return internal_field.update();
    }

    int run ()
    {
        int res = 0;

        while (!res)
            res = run_once();

        return 0;
    }

    const std::vector <int> &get_stash () const
    {
        return internal_field.get_stash();
    }

    void print (int default_height)
    {
        int a;

        if (rank != 0)
        {
            MPI_Status status;
            MPI_Recv (&a, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        }

        std::vector <int> tmp(width);

        for (int i = 0; i < height; ++i)
        {
            internal_field.get_str (tmp.begin(), i);

            std::cout << rank << "-" << rank * default_height + i << ") ";

            for (int j = 0; j < width; ++j)
                std::cout << tmp[j] << " ";

            std::cout << std::endl;
        }
        
        if (rank != commsize - 1)
            MPI_Send (&a, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
};