/*
 * Implementation of the codes wwith local varrriables
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct tinfo {
    int * array;
    int size;
};

void * counter(void * targs)
{
    struct tinfo * loc_info = (struct tinfo *) targs;

    for (int i = 0; i < loc_info->size; ++i)
        for (int j = 0; j < i; ++j)
            (loc_info->array[i])++;
}

void check_array(int * array, int size, int th_num)
{
    int errors = 0;

    puts("Checking...");
    
    for (int i = 0; i < size; ++i)
    {
        if (array[i] != th_num * i)
        {
            errors++;
            printf("%d: %d should be %d\n", i, array[i], th_num * i);
        }
    }

    printf("%d errors\n", errors);
}

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        printf("Usage: <%s> <th_num> <size>\n", argv[0]);
        exit(1);
    }

    int th_num = strtol(argv[1], NULL, 10);
    int size = strtol(argv[2], NULL, 10);
    int * array = calloc(size, sizeof(int));
    struct tinfo info[th_num];
    pthread_t tid[th_num];

    for (int i = 0; i < th_num; ++i)
    {
        info[i].array = array;
        info[i].size = size;
        
        pthread_create(&tid[i], NULL, counter, &info[i]);
    }

    for (int i = 0; i < th_num; ++i)
        pthread_join(tid[i], NULL);

    check_array(array, size, th_num);

    return EXIT_SUCCESS;
}

/*
 * The threads recieve the same memory location
 * which means that without proper locking mechnisms
 * it will return the wrong result, as it should happen
 * when running the bianry
*/
