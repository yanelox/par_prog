#include <thread>
#include <sstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <atomic>

int main (int argc, char *argv[])
{
    if (argc > 2)
    {
        std::cout << "Слишком много аргументов" << std::endl;
        return 0;
    }

    int thread_count = 2;

    if (argc == 2)
    {
        std::stringstream stream(argv[1]);
        stream >> thread_count;
    }

    std::vector <std::thread> threads(thread_count);
    std::atomic <int> flag(42);

    auto func = [&flag](int i)
    {
        while (flag.load() !=  42 + i) {}

        std::cout << "my number = " << i << ", flag = " << flag.load() << std::endl;

        flag.store(42 + i + 1);
    };

    for (int i = 0; i < thread_count; ++i)
        threads[i] = std::thread (func, i);

    for (auto &thread:threads)
        thread.join();
}