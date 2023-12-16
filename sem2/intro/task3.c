#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main ()
{
    int result = 0;

    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        int thread_count = omp_get_num_threads();

        #pragma omp for ordered
            for (int i = 0; i < thread_count; i++)
                #pragma omp ordered
                {
                    result += 1;
                    printf ("%d: result = %d\n", thread_num, result);
                }
    }
}
