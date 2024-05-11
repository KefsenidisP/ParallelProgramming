#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ITERS 1000

// Without the locks, it may not return 0
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct tinfo {
    int id;
    int * total;
};

void * add(void * targs)
{
    struct tinfo * loc_targs = (struct tinfo *) targs;

    for (int i = 0; i < ITERS; ++i)
    {
        pthread_mutex_lock(&mutex);
        (*(loc_targs->total)) += 1;
        pthread_mutex_unlock(&mutex);
    }

    printf("Thread with id = %d, total = %d\n", loc_targs->id, *(loc_targs->total));
}

void * sub(void * targs)
{
    struct tinfo * loc_targs = (struct tinfo *) targs;
    
    for (int i = 0; i < ITERS; ++i)
    {
        pthread_mutex_lock(&mutex);
        (*(loc_targs->total)) -= 1; 
        pthread_mutex_unlock(&mutex);
    }

    printf("Thread with id = %d, total = %d\n", loc_targs->id, *(loc_targs->total));
}

int main(int argc, char * args[])
{
    int total = 0;

    if (argc != 2)
    {
        printf("Usage: <%s> <th_num>", args[0]);
        exit(1);
    }

    int th_num = strtol(args[1], NULL, 10);
    struct tinfo targs[th_num * 2];
    pthread_t tid[th_num * 2];

    for (int i = 0; i < th_num; ++i)
    {
        targs[i].id = i;
        targs[i].total = &total;
        pthread_create(&tid[i], NULL, add, &targs[i]);

        targs[th_num + i].id = th_num + i;
        targs[th_num + i].total = &total;
        pthread_create(&tid[i + th_num], NULL, sub, &targs[i + th_num]);
    }

    /*
     * If this step is ignored, then there is a chance, that the 
     * main process will call exit and thus killing its children
     * threads
    */
    for (int i = 0; i < (2 * th_num); ++i)
        pthread_join(tid[i], NULL);

    printf("Total must be 0: %d\n", total);

    return EXIT_SUCCESS;
}
