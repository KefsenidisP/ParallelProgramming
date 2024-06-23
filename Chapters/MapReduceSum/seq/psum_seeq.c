#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    clock_t start, end;

    if (argc != 2)
    {
        printf("Usage: <%s> <thread_num> <size>", argv[0]);
        exit(1);
    }

    N = strtol(argv[1], NULL, 10);

    srand(time(NULL));
    X = (int *) array_init(&N);

    start = clock();;
    for (i = 0; i < N; ++i)
        sum = sum + X[i];

    end = clock();

    double time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Time elapsed: %f\n", time);

    return 0;
}
