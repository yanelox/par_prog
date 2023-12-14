#include <thread>
#include <sstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <mutex>

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "первый аргумент - N, второй - число тредов(если не задано, то 2 треда)" << std::endl;;
        return 0;
    }

    int thread_count = 2;

    std::stringstream stream1(argv[1]);
    int N;
    stream1 >> N;

    if (argc == 3)
    {
        std::stringstream stream2(argv[2]);
        stream2 >> thread_count;

        if (thread_count < 1)
        {
            std::cout << "Число тредов не может быть меньше 1" << std::endl;

            return 0;
        }
    }

    if (thread_count > N)
    {
        std::cout << "Число тредов не может быть больше N" << std::endl;

        return 0;
    }

    int default_size = N / thread_count;
    int last_size = N - default_size * (thread_count - 1);

    double res = 0;

    std::vector <std::thread> threads(thread_count);
    std::mutex mutex;

    auto count_sum = [&res, &mutex, default_size, last_size](int i, int is_last)
    {
        int start = 1 + default_size * i;
        int end = 1 + default_size * i + ((is_last == 1) ? last_size : default_size);
        assert (start > 0 and start <= end);

        double tmp_res = 0.0;

        for (int i = start; i < end; ++i)
            tmp_res += 1.0 / i;

        std::lock_guard <std::mutex> guard(mutex);

        res += tmp_res;
    };

    for (int i = 0; i < thread_count; ++i)
        if (i != thread_count - 1)
            threads[i] = std::thread (count_sum, i, 0);
        else
            threads[i] = std::thread (count_sum, i, 1);

    for (auto &thread:threads)
        thread.join();

    std::cout << "sum = " << res << std::endl;
}