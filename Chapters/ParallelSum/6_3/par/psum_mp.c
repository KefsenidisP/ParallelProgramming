#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX_NUM 100
// #define MAX_SIZE 1000000

void * array_init(int * size)
{
    int * array;

    array = (int *) malloc((*size) * sizeof(int));

    for (int i = 0; i < (*size); ++i)
    {
        array[i] = rand() % MAX_NUM;
    }

    return array;
}

int main(int argc, char* argv[])
{
    int sum = 0;
    int *X;
    int N, th_num, i;
    double start, end;

    if (argc != 3)
    {
        printf("Usage: <%s> <thread_num> <size>", argv[0]);
        exit(1);
    }

    th_num = strtol(argv[1], NULL, 10);
    N = strtol(argv[2], NULL, 10);

    omp_set_num_threads(th_num);
    omp_set_nested(1);

    srand(time(NULL));
    X = (int *) array_init(&N);

    start = omp_get_wtime();
#pragma omp parallel for default(shared) private(i) reduction(+:sum) 
    for (i = 0; i < N; ++i)
        sum = sum + X[i];

    end = omp_get_wtime();

    double time_par = end - start;

    printf("Time elapsed: %f\n", time_par);

    return 0;
}
