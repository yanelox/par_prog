#include <thread>
#include <sstream>
#include <iostream>
#include <vector>

void HelloWorld_func (int number)
{
    std::stringstream tmp;
    tmp << number << ": Hello, world!" << std::endl;
    std::cout << tmp.str();
}

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Добавьте число потоков в аргументы или удалите лишнее" << std::endl;
        return 0;
    }

    std::stringstream input{argv[1]};

    int thread_count;
    input >> thread_count;

    std::vector <std::thread> threads(thread_count);

    for (int i = 0; i < thread_count; ++i)
        threads[i] = std::thread (HelloWorld_func, i);

    for (auto &thread:threads)
        thread.join();
}