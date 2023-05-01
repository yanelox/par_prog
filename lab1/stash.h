#include <vector>
#include <mpi.h>

template <typename T>
class MStash
{
    std::vector <std::vector <T>> internal;

    int x_size; // count of rows
    int y_size; // count of columns

    public:

    MStash () = default;

    MStash (int x_size_, int y_size_): x_size(x_size_), y_size(y_size_)
    {
        internal.resize(x_size);

        for (auto &&line:internal)
            line.resize(y_size);
    }

    const std::vector <T> & operator () (int i)
    {
        return internal[i];
    }

    T& operator () (int i, int j)
    {
        return internal[i][j];
    }
};