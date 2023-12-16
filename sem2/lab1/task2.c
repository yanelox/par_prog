#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define ISIZE 1000
#define JSIZE 1000

// 2Г

int main(int argc, char **argv)
{
    assert(argc == 3);

    char* filename = argv[1];
    int num_threads = atoi (argv[2]);

    omp_set_num_threads(num_threads);

    double a[ISIZE][JSIZE];

    FILE *ff;
    for (int i = 0; i < ISIZE; i++)
        for (int j = 0; j < JSIZE; j++)
            a[i][j] = 10 * i + j;

    int i, j;

    //начало измерения времени
    double time_start = omp_get_wtime();

    for (i = 0; i < ISIZE - 3; i++)
    {
        #pragma omp parallel for
            for (j = 2; j < JSIZE; j++)
                a[i][j] = sin(0.1 * a[i + 3][j - 2]);

    }

    //окончание измерения времени
    double time_end = omp_get_wtime();

    printf ("time=%f\n", time_end - time_start);

    ff = fopen(filename,"w");

    for(int i = 0; i < ISIZE; i++)
        for (int j = 0; j < JSIZE; j++)
            fprintf(ff,"%d %d %f\n", i, j, a[i][j]);

    fclose(ff);
}