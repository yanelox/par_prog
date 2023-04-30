#include "worker.h"

class chief
{
    int full_height;

    int width;
    int height;

    int rank;
    int commsize;

    int default_height;

    worker alone;

    public:

    chief (int width_, int height_, int rank_, int commsize_, std::vector <int> &init):
        rank (rank_), commsize (commsize_)
    {
        if (width_ < height_)
        {
            full_height = width_;
            width = height_;
        }

        else
        {
            full_height = height_;
            width = width_;
        }

        default_height = full_height / commsize;

        if ((rank == commsize - 1) && (full_height % commsize != 0))
        {
            height = full_height - default_height * (commsize - 1);
            assert (height > 0);
        }

        else
            height = default_height;

        alone = worker (width, height, rank, commsize);
        alone.init_field (init.begin() + rank * default_height * width);
    }

    int run_once ()
    {
        return alone.run_once();
    }

    int run ()
    {
        return alone.run();
    }

    const std::vector <int> &get_stash () const
    {
        return alone.get_stash();
    }

    void print ()
    {
        alone.print(default_height);
    }
};