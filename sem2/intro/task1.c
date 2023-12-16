#include <omp.h>
#include <stdio.h>

int main ()
{
    #pragma omp parallel
    {
        int num = omp_get_thread_num();
        int all = omp_get_num_threads();
        printf("Hello, world! %d %d\n", num, all);
    }
}
