/* Vector Addition a = b + c                       */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

struct tinfo {
    int size;
    int tid;
    double * a;
    double * b;
    double * c;
};

int th_num; 

void * array_add(void *targs)
{
    struct tinfo *loc_targs = (struct tinfo *) targs;
    int start = loc_targs->tid * (loc_targs->size / th_num);
    int end = start + (loc_targs->size / th_num);
    
    if (loc_targs->tid = th_num - 1)
        end = loc_targs->size;
    
    for (int i = start; i < end; i++)
        loc_targs->a[i] = loc_targs->b[i] + loc_targs->c[i];
}

int main(int argc, char* args[])
{
    if (argc != 3)
    {
        printf("Usage: <%s> <array_size> <th_num>", args[0]);
        exit(1);
    }

    th_num = strtol(args[2], NULL, 10);
    
    pthread_t tid[th_num];
    struct tinfo targs[th_num];
    int size = strtol(args[1], NULL, 10);

    double * a = (double *) malloc(size * sizeof(double));
    double * b = (double *) malloc(size * sizeof(double));
    double * c = (double *) malloc(size * sizeof(double));

    for (int i = 0; i < size; i++)
    {
        a[i] = 0.0;
        b[i] = 1.0;
        c[i] = 0.5;
    }

    time_t start = time(NULL);
    for (int i = 0; i < th_num; i++)
    {
        targs[i].size = size;
        targs[i].tid = i;
        targs[i].a = a;
        targs[i].b = b;
        targs[i].c = c;

        pthread_create(&tid[i], NULL, array_add, &targs);
    }

    for (int i = 0; i < th_num; i++)
        pthread_join(tid[i], NULL);
    time_t end = time(NULL);
    // debugging
    // for (int i = 0; i < size; i++)
        // printf("%f\n", a[i]);

    // Use the folloowing wwhen using clock, instead of time
    // double time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Time taken: %d secs\n", end - start);

    return EXIT_SUCCESS;
}
