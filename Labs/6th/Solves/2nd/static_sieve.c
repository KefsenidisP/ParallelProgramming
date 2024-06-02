/*
 * Static assignment for each thread on the sieve algorithm
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

#define synchronized(obj) \
    while (pthread_mutex_trylock(&obj)); \
    for (int locked = 1; locked; locked = pthread_mutex_unlock(&obj))

struct arguments {
    int rank;
    int threads;
    int limit;
    int size;
    int * prime;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double get_time () {
	struct timeval t;
        
        gettimeofday(&t, NULL); 
        return (t.tv_sec + t.tv_usec/1000000.0);
}

// The parallelized part
void * sieve(void * targs)
{
    struct arguments * args = (struct arguments *) targs;
    int start, end;

    start = ((args->limit) / (args->threads)) * (args->rank) + 2;
    end = ((args->limit) / (args->threads)) + start;
    if (args->rank == (args->threads) - 1)
        end = (args->limit);

    for (int p = start; p <= end; ++p)
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
    double time = get_time();

    limit = (int) sqrt(size) + 1;
    for (int i = 0; i < th_num; ++i)
    {
        targs[i].rank = i;
        targs[i].threads = th_num;
        targs[i].limit = limit;
        targs[i].size = size;
        targs[i].prime = prime;

        pthread_create(&tid[i], NULL, sieve, &targs[i]);
    }

    for (int i = 0; i < th_num; ++i)
        pthread_join(tid[i], NULL);

    time = get_time() - time;

    int count = 0;
    for(int i = 2; i <= size; i++) 
        if (prime[i] == 1)
            count++;

    printf("Number of primes: %d\n", count);
    printf("Time in seconds: %f\n", time);

    return EXIT_SUCCESS;
}
