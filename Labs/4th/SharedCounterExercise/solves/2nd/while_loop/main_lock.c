#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct tinfo {
    int * array;
    int size;
    int * counter;
    pthread_mutex_t * mutex; 
};

// Change the counter and incrrease elm
void inc(int * elm, int * counter, pthread_mutex_t * mutex)
{
    (*elm)++;
    (*counter)++;
    pthread_mutex_unlock(mutex);
}

// Get the element (read the counter)
void get(int * array, int * counter, pthread_mutex_t * mutex)
{
    pthread_mutex_lock(mutex);
    inc(&array[*counter], counter, mutex);
}

void * counter_fun(void * targs)
{
    struct tinfo * loc_info = (struct tinfo *) targs;

    while (1)
    {
        if (*(loc_info->counter) >= loc_info->size)
            break;
        
        get(loc_info->array, loc_info->counter, loc_info->mutex);
    }
}

void check_array(int * array, int size)
{
    int errors = 0;

    puts("Checking...");
    
    for (int i = 0; i < size; ++i)
    {
        if (array[i] != 1)
        {
            errors++;
            printf("%d: %d should be 1\n", i, array[i]);
        }
    }

    printf("%d errors\n", errors);
}

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        printf("Usage: <%s> <th_num> <size>\n");
        exit(1);
    }

    int th_num = strtol(argv[1], NULL, 10);
    int size = strtol(argv[2], NULL, 10);
    int counter = 0;
    int * array = calloc(size, sizeof(int));

    struct tinfo info[th_num];
    pthread_t tid[th_num];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    for (int i = 0; i < th_num; ++i)
    {
        info[i].array = array;
        info[i].size = size;
        info[i].counter = &counter;
        info[i].mutex = &mutex;
        
        pthread_create(&tid[i], NULL, counter_fun, &info[i]);
    }

    for (int i = 0; i < th_num; ++i)
        pthread_join(tid[i], NULL);

    check_array(array, size);

    return EXIT_SUCCESS;
}

/*
 * The mutex memory location iis shared among
 * the threads, due to the targs argument of the function
 * so it prints the correct results.
*/
