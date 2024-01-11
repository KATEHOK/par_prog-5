#define _CRT_RAND_S
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "mpi.h"
#include <windows.h>

void qwerty(unsigned int* full_arr, const int arr_id, const int full_arr_size, const char* filename)
{
    int numtasks, rank, result;
    unsigned int max_num = 0;

    LARGE_INTEGER frequency;
    LARGE_INTEGER start_time;
    LARGE_INTEGER end_time;

    QueryPerformanceFrequency(&frequency);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    int arr_size = full_arr_size / numtasks + 1;
    unsigned int* arr = (unsigned int*)malloc(sizeof(unsigned int) * arr_size);

    if (rank == 0) {
        int my_size = 0;
        QueryPerformanceCounter(&start_time);

        for (int dst_rank = 1; dst_rank < numtasks; ++dst_rank) {
            MPI_Send(full_arr + (dst_rank - 1) * arr_size, arr_size, MPI_UNSIGNED, dst_rank, dst_rank, MPI_COMM_WORLD);
        }

        for (int i = (numtasks - 1) * arr_size; i < full_arr_size; ++i) arr[my_size++] = full_arr[i];

        arr_size = my_size;
    }
    else {
        MPI_Status status;
        MPI_Recv(arr, arr_size, MPI_UNSIGNED, 0, rank, MPI_COMM_WORLD, &status);
    }

    for (int i = 0; i < arr_size; ++i) if (arr[i] > max_num) max_num = arr[i];

    MPI_Reduce(&max_num, &result, 1, MPI_UNSIGNED, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        QueryPerformanceCounter(&end_time);
        double elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart * 1e6;
        printf("Array id: %d\tMax is: %u\tTime: %.2f\n", arr_id, result, elapsed_time);
        
        FILE* file;
        fopen_s(&file, filename, "a");
        fprintf(file, "%d\t%u\t%f\n", arr_id, result, elapsed_time);
        fclose(file);
    }

    free(arr);
}

int main(int argc, char** argv)
{
    const int arr_size = 10000000;
    const int arrs = 10;
    const char filename[] = "data.txt";
    unsigned int* arr = (unsigned int*)malloc(sizeof(unsigned int) * arr_size * arrs);

    printf("OpenMP: %d\n", _OPENMP);
    int i;
    #pragma omp parallel for private(i)
    for (i = 0; i < arr_size * arrs; ++i) rand_s(arr + i);
    
    MPI_Init(&argc, &argv);

    for (int arr_id = 0; arr_id < arrs; ++arr_id) {
        MPI_Barrier(MPI_COMM_WORLD);
        qwerty(arr + arr_id * arr_size, arr_id, arr_size, filename); 
    }

    MPI_Finalize();

    free(arr);

	return 0;
}

//Проект > Свойства > С++ > Общие > Дополнительные каталоги включаемых файлов > "E:\MS_MPI\Include\x64"
//Проект > Свойства > С++ > Общие > Дополнительные каталоги включаемых файлов > "E:\MS_MPI\Include"
//Проект > Свойства > Компоновщик > Все параметры > Дополнительные каталоги библиотек > "E:\MS_MPI\Lib\x64"
//Проект > Свойства > Компоновщик > Все параметры > Дополнительные зависимости > "E:\MS_MPI\Lib\x64\msmpi.lib"

//PS E:\Inst\3 course\5 sem\par_prog\labs\5\par_prog-5\report> mpiexec -n 2 ..\..\lab5\x64\Debug\lab5.exe