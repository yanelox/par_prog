#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>

#include "worker.hpp"

double foo (double x)
{
    return std::cos(1 / std::pow (x - 1, 2));
}

int main (int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Первый аргумент - число потоков, второй - погрешность интегрирования" << std::endl;
        return 0;
    }
    
    std::stringstream stream1(argv[1]);
    int count_threads;
    stream1 >> count_threads;

    std::stringstream stream2(argv[2]);
    double eps;
    stream2 >> eps;

    Worker worker(count_threads);

    Integral integral(foo, 0, 0.99, eps);
    worker.run(integral);

    int precision = 0;

    while (eps < 1)
    {
        ++precision;
        eps *= 10;
    }
    
    std::cout << std::setprecision(precision) << integral.res << std::endl;
}