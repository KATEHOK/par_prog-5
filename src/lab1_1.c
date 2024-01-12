#define _CRT_RAND_S
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void qwerty(const int threads, const int* array, const unsigned int count)
{
    double start, end;
    int max = -1;
    int i;
    start = omp_get_wtime();
    #pragma omp parallel num_threads(threads) reduction(max: max) private(i)
    {
        #pragma omp for
        for (i = 0; i < count; ++i) if (array[i] > max) max = array[i];
        //printf("%2d: My lmax is: %d;\n", omp_get_thread_num(), max);

    }
    end = omp_get_wtime();
    printf("Max is: %d\nTime: %f\n======\n", max, end - start);
}

int main(int argc, char** argv)
{
    const unsigned int c = 100000000;

    int* arr = (int*)malloc(10 * c * sizeof(int));
    if (arr == NULL) return 1;

    printf("OpenMP: %d\n", _OPENMP);
    for (int i = 0; i < 10 * c; i++) rand_s(arr + i);

    for (int th = 1; th <= 16; ++th)
        for (int i = 0; i < 10; ++i) { 
            printf("\n======\nThreads: %2d; Array: %d\n", th, i);
            qwerty(th, arr + i * c, c); 
        }

    free(arr);

	return 0;
}