/*
 * Static assignment for each thread on the sieve algorithm
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define synchronized(obj) \
    while (pthread_mutex_trylock(&obj)); \
    for (int locked = 1; locked; locked = pthread_mutex_unlock(&obj))

struct arguments {
    int rank;
    int threads;
    int limit;
    int size;
    int * p; // p becomes global among threads
    int * prime;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Increases global p and returns it to a local variable
void get_p(int * p, int * loc_p)
{
    synchronized(mutex)
    {
        (*p)++;
        (*loc_p) = (*p);
    }
}

// The parallelized part
void * sieve(void * targs)
{
    struct arguments * args = (struct arguments *) targs;
    int p;

    for (get_p(args->p, &p); p <= (args->limit); get_p(args->p, &p))
        if ((args->prime[p]) == 1)
            for (int i = p * p; i <= (args->size); i += p)
                (args->prime[i]) = 0;
}

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        printf("Usage: <%s> <size> <th_num>\n");
        exit(1);
    }

    int size;
    int th_num;
    int * prime;
    int limit;
    int p = 1; // Initialize to 1, because get_p increases it

    size = strtol(argv[1], NULL, 10);
    th_num = strtol(argv[2], NULL, 10);

    // Negative value results in smallest size chunk
    prime = (int *) malloc(size * sizeof(int));

    // Some checking
    if (size <= 0 || th_num <= 0)
    {
        printf("Negative value detected. Positive values required\n");
        exit(2);
    }

    for (int i = 2; i <= size; ++i)
        prime[i] = 1;

    pthread_t tid[th_num];
    struct arguments targs[th_num];
    clock_t time_clock = clock();

    limit = (int) sqrt(size) + 1;
    for (int i = 0; i < th_num; ++i)
    {
        targs[i].rank = i;
        targs[i].threads = th_num;
        targs[i].limit = limit;
        targs[i].size = size;
        targs[i].p = &p;
        targs[i].prime = prime;

        pthread_create(&tid[i], NULL, sieve, &targs[i]);
    }

    for (int i = 0; i < th_num; ++i)
        pthread_join(tid[i], NULL);

    time_clock = clock() - time_clock;

    double time = ((double) time_clock) / CLOCKS_PER_SEC;

    int count = 0;
    for(int i = 2; i <= size; i++) 
        if (prime[i] == 1)
            count++;

    printf("Number of primes: %d\n", count);
    printf("Time in seconds: %f\n", time);

    return EXIT_SUCCESS;
}
