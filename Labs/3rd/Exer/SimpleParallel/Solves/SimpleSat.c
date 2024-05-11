/*
 * There are problems with the stdout handling, 
 * which of course can't be solved
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define SIZE 28
#define ITERS (1 << SIZE)

struct tinfo {
    int id;
};

int th_num;
char * result; // The global variable. Handle with mutexes

void print_result(int * v, int z)
{
    char * result = (char *) malloc(0x128); // Just a big chunk, just in case. (In tcache)
    int length = snprintf( NULL, 0, "%u", z ); // This format, returns the length
    
    snprintf(result, length + 1, "%u", z);

    for (int i = 0; i < SIZE; ++i)
    {
        if (v[i]) 
            strncat(result, " 1", 2);
        else
            strncat(result, " 0", 2);
    }

    free(result);

    // puts(result);

}

void check_circuit(int z)
{
    int v[SIZE];  

    for (int i = SIZE - 1; i >= 0; --i) 
        v[i] = ((z & (1 << i)) != 0);

    // Check the ouptut of the circuit for input v
    int value = 
       (  v[0]  ||  v[1]  )
    && ( !v[1]  || !v[3]  )
    && (  v[2]  ||  v[3]  )
    && ( !v[3]  || !v[4]  )
    && (  v[4]  || !v[5]  )
    && (  v[5]  || !v[6]  )
    && (  v[5]  ||  v[6]  )
    && (  v[6]  || !v[15] )
    && (  v[7]  || !v[8]  )
    && ( !v[7]  || !v[13] )
    && (  v[8]  ||  v[9]  )
    && (  v[8]  || !v[9]  )
    && ( !v[9]  || !v[10] )
    && (  v[9]  ||  v[11] )
    && (  v[10] ||  v[11] )
    && (  v[12] ||  v[13] )
    && (  v[13] || !v[14] )
    && (  v[14] ||  v[15] )
    && (  v[14] ||  v[16] )
    && (  v[17] ||  v[1]  )
    && (  v[18] || !v[0]  )
    && (  v[19] ||  v[1]  )
    && (  v[19] || !v[18] )
    && ( !v[19] || !v[9]  )
    && (  v[0]  ||  v[17] )
    && ( !v[1]  ||  v[20] )
    && ( !v[21] ||  v[20] )
    && ( !v[22] ||  v[20] )
    && ( !v[21] || !v[20] )
    && (  v[22] || !v[20] );
    
    // If output == 1 print v and z
    if (value)
        print_result(v, z); 
}

void * main_thread_fun(void * targs)
{
    struct tinfo * loc_targs = (struct tinfo *) targs;
    int start = loc_targs->id * (ITERS / th_num);
    int end = start + (ITERS / th_num);

    for (int i = start; i < end; ++i)
        check_circuit(i);
}

int main(int argc, char * args[])
{
    if (argc != 2)
    {
        printf("Usage: <%s> <th_num>\n", args[0]);
        exit(1);
    }

    th_num = strtol(args[1], NULL, 10);
    
    pthread_t tid[th_num];
    struct tinfo targs[th_num];
    
    time_t start = time(NULL);
    for (int i = 0; i < th_num; ++i)
    {
        targs[i].id = i;
        pthread_create(&tid[i], NULL, main_thread_fun, &targs[i]);
    }

    for (int i = 0; i < th_num; ++i)
        pthread_join(tid[i], NULL);
    time_t end = time(NULL);

    // double time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nTime taken: %d seeconds\n", end - start);

    return EXIT_SUCCESS;
}
