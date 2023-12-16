#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main (int argc, char* argv[])
{
    assert (argc == 2);

    int n = atoi (argv[1]);
    float sum = 0;

    int max_thread_num = omp_get_max_threads();
    float* results = calloc(max_thread_num, sizeof(float));

    int real_threads_count = 0;

    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();

        #pragma omp master
            real_threads_count = omp_get_num_threads();

        #pragma omp for
            for (int i = 1; i <= n; i++)
                results[thread_num] += 1.0 / i;

        #pragma omp master
            for (int i = 0; i < real_threads_count; ++i)
                sum += results[i];
    }

    free(results);

    printf ("Result: %f\n", sum);
}
