#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct tinfo {
    int * array;
    int size;
    int * counter;
};

void * counter_fun(void * targs)
{
    struct tinfo * loc_info = (struct tinfo *) targs;

    while (1)
    {
        if (*(loc_info->counter) >= loc_info->size)
            break;
        
        int i = (*(loc_info->counter));
        
        (loc_info->array[i])++;
        (*(loc_info->counter))++;
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

    for (int i = 0; i < th_num; ++i)
    {
        info[i].array = array;
        info[i].size = size;
        info[i].counter = &counter;
        
        pthread_create(&tid[i], NULL, counter_fun, &info[i]);
    }

    for (int i = 0; i < th_num; ++i)
        pthread_join(tid[i], NULL);

    check_array(array, size);

    return EXIT_SUCCESS;
}

/*
 * It is still wrong because both the counter and the array 
 * memory location of the main thread are used forr all 
 * threads. It needs proeerr lock mechanisms
*/
