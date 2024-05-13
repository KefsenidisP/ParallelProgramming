#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUM 100

int N; // number of elements in array X
int *X;
int th_num; // Number of threads
pthread_t * tHandles;
int * gSum; // global storage for partial results

void * Summation(void *pArg)
{
    int tNum = *((int *) pArg);
    int lSum = 0;
    int start, end;
    
    start = (N / th_num) * tNum;
    end = (N / th_num) + start;
    
    if (tNum >= (th_num - 1)) end = N;
    
    for (int i = start; i < end; i++)
        lSum += X[i];
    
    gSum[tNum] = lSum;
}

void initializeArray(int * array, int size)
{
    for (int i = 0; i < size; ++i)
    {
        array[i] = rand() % MAX_NUM;
        // printf("%d ", array[i]);
    }
}

int main(int argc, char* argv[])
{
    int j, sum = 0;
    
    if (argc != 3)
    {
        printf("Usage: <%s> <threadNum> <arraySize>\n", argv[0]);
        exit(1);
    }

    srand(time(NULL));

    th_num = strtol(argv[1], NULL, 10);
    N = strtol(argv[2], NULL, 10);

    int id[N];
    
    tHandles = (pthread_t *) malloc(th_num * sizeof(pthread_t));
    gSum = (int *) malloc(th_num * sizeof(int));
    X = (int *) malloc(N * sizeof(int));

    initializeArray(X, N); // get values into X array; not shown
    
    clock_t time_clock = clock();
    for (j = 0; j < th_num; j++) 
    {
        id[j] = j;
        pthread_create(&tHandles[j], NULL, Summation, &id[j]);
    }

    for (j = 0; j < th_num; j++) 
    {
        pthread_join(tHandles[j], NULL);
        sum += gSum[j];
    }
    time_clock = clock() - time_clock;

    double time_taken_par = ((double) time_clock) / CLOCKS_PER_SEC;
    
    printf("The sum of array elements is %d\n", sum);
    printf("Time taken for parallel algorithm: %f\n", time_taken_par);
    

    // Single process testing
    int procs = th_num; // Just keep the previous value here
    th_num = 1;
    tHandles = (pthread_t *) malloc(th_num * sizeof(pthread_t));

    time_clock = clock();
    pthread_create(&tHandles[0], NULL, Summation, &id[0]);
    pthread_join(tHandles[0], NULL);
    sum = gSum[0];
    time_clock = clock() - time_clock;
    double time_taken_single = ((double) time_clock) / CLOCKS_PER_SEC;

    printf("The sum of array elements is %d\n", sum);
    printf("Time taken for single process: %f\n", time_taken_single);

    double speedup = time_taken_single / time_taken_par;
    double efficiency = (time_taken_single / procs) / time_taken_par;

    printf("\n-----RESULTS-----\n");
    printf("The speedup is: %f\n", speedup);
    printf("Efficiency is: %f\n", efficiency);

    return 0;
}
