#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// The first loop will run until it catches a lock,
// while the second will run only once, to unlock the 
// lock
#define synchronized(obj) \
    while (pthread_mutex_trylock(&obj)); \
    for (int locked = 1; locked; locked = pthread_mutex_unlock(&obj))

struct tinfo {
    int size;
    int * num;
};

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void * inc1(void * targs)
{
    struct tinfo * info = (struct tinfo *) targs;
    
    for (int i = 0; i < info->size; ++i)
    {
        synchronized(mutex1)
            (*(info->num))++;
    }
}

void * inc2(void * targs)
{
    struct tinfo * info = (struct tinfo *) targs;
    
    for (int i = 0; i < info->size; ++i)
    {
        synchronized(mutex2)
            (*(info->num))++;
    }
}
int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        printf("Usage: <%s> <th_num> <times>", argv[0]);
        exit(0);
    }

    int th_num = strtol(argv[1], NULL, 10);
    int times = strtol(argv[2], NULL, 10);
    int num1 = 0;
    int num2 = 0;

    pthread_t tid[th_num];
    struct tinfo info[th_num];

    for (int i = 0; i < th_num; ++i)
    {
        info[i].size = times;

        if (i % 2 == 0)
        {
            info[i].num = &num1;
            pthread_create(&tid[i], NULL, inc1, &info[i]);
        }
        else
        {
            info[i].num = &num2;
            pthread_create(&tid[i], NULL, inc2, &info[i]);
        }
    }

    for (int i = 0; i < th_num; ++i)
        pthread_join(tid[i], NULL);

    printf("Num1 is: %d\n", num1);
    printf("Num2 is: %d\n", num2);

    return 0;
}
