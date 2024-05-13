#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUM 100

int * prefix_scan(int * array, int size)
{
    int * in_prefix;

    in_prefix = (int *) malloc(size * sizeof(int));

    in_prefix[0] = array[0];
    for (int i = 1; i < size; i++)
        in_prefix[i] = in_prefix[i-1] + array[i];
    
    return in_prefix;
}

void array_init(int * array, int size)
{
    for (int i = 0; i < size; ++i)
        array[i] = rand() % MAX_NUM;
}

void print_array(int * array, int size)
{
    for (int i = 0; i < size; ++i)
        printf("%d ", array[i]);
    printf("\n");
}

int main(int argc, char * argv[])
{
    int * a;
    int n;

    if (argc != 2)
    {
        printf("Usage: <%s> <size>", argv[0]);
        exit(1);
    }

    n = strtol(argv[1], NULL, 10);

    a = (int *) malloc(n * sizeof(int));

    array_init(a, n);
    // print_array(a, n);

    clock_t time_clock = clock();
    a = prefix_scan(a, n);
    time_clock = clock() - time_clock;
    // print_array(a, n);
    
    double time_elapsed = ((double) time_clock) / CLOCKS_PER_SEC;

    printf("Time elapsed: %f\n", time_elapsed);

    return EXIT_SUCCESS;
}
