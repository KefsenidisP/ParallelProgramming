// #include <windows.h>
// #include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/*
 * The algorithm is bassically divided in some steps, which mst b done sequantially,
 * with the use of signals. Of course the loops can still be parallelized.
 */

#define MAX_NUM 100
#define ALLOC_SZ(times, type) (times * sizeof(type))

int th_num; // Number of threads
int N; // Size of array A
int * A; // The array of random integers
int * waiting_th; // The threead is waiting for signal or not

/*
 * The below two, are helper arrays, for calculating the inclusive prefix of array A.
 * The algorithm divides array A in subarrays, so in order to get the A[start - 1] element,
 * inTotals array is used. So each index will have the last element of the after the prefix scan,
 * of the subarray. Then the outTotals is used in order to calculate the missing sum of each thread.
 */
int * inTotals; 
int * outTotals;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * prefixScan(void * pArg)
{
    int tNum = *((int *) pArg); // Thread num
    int start, end, i;
    int lPrefixTotal;
    
    start = (N / th_num) * tNum;
    end = (N / th_num) * (tNum + 1);
    if (tNum == (th_num - 1)) end = N;
    
    // Step 1
    for (i = start+1; i < end; i++)
        A[i] = A[i-1] + A[i];
    
    inTotals[tNum] = A[end-1];

    /*
     * Once all threads are waiting step 1 will be completed and
     * all threads will be waiting for a broadcast to start step 3.
     */
    pthread_mutex_lock(&lock);
    waiting_th[tNum] = 1; // Signal complete
    pthread_cond_wait(&cond, &lock); // Wait
    pthread_mutex_unlock(&lock);
    waiting_th[tNum] = 0;

    //SetEvent(doneStep1[tNum]); //signal completion of Step 1
    
    // wait for completion of Step 2
    //WaitForSingleObject(doneStep2, INFINITE);
    
    // Step 3
    lPrefixTotal = outTotals[tNum];
    for (i = start; i < end; i++)
        A[i] = lPrefixTotal + A[i];
    
    return 0;
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

int main(int argc, char* argv[])
{
    int i, j;
    int * id;
    pthread_t * handle;

    if (argc != 3)
    {
        printf("Usage: <%s> <th_num> <size>\n", argv[0]);
        exit(1);
    }

    // Initialization section
    srand(time(NULL));

    th_num = strtol(argv[1], NULL, 10);
    N = strtol(argv[2], NULL, 10);

    handle = (pthread_t *) malloc(ALLOC_SZ(th_num, pthread_t));
    id = (int *) malloc(ALLOC_SZ(th_num, int));
    inTotals = (int *) malloc(ALLOC_SZ(th_num, int));
    outTotals = (int *) malloc(ALLOC_SZ(th_num, int));
    waiting_th = (int *) malloc(ALLOC_SZ(th_num, int));

    A = (int *) malloc(ALLOC_SZ(N, int));


    array_init(A, N); // get values into A array; not shown
    // print_array(A, N);

    // Thread creation
    clock_t time_clock = clock();
    for (i = 0; i < th_num; ++i) 
    {
        id[i] = i;
        pthread_create(&handle[i], NULL, prefixScan, &id[i]);
    }
    
    // wait for Step 1 completion
    i = 0;
    while (1)
    {
        if (waiting_th[i] == 1)
            i++;
        if (i == th_num)
            break;
    }

    // Step 2
    outTotals[0] = 0;
    for (j = 1; j < th_num; ++j)
        outTotals[j] = outTotals[j-1] + inTotals[j-1];
    
    // Wake all threads, after step 2
    pthread_cond_broadcast(&cond);
    
    // wait for completion of Step 3
    // Actually join threads (steo 3)
    for (i = 0; i < th_num; ++i)
        pthread_join(handle[i], NULL);
    time_clock = clock() - time_clock;

    double time_elapsed = ((double) time_clock) / CLOCKS_PER_SEC;

    printf("Time elapsed: %f\n", time_elapsed);

    // print_array(A, N);

    return 0;
}
