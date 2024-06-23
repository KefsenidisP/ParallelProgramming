/*
 * The code is a parallel sum implementation.
 *
 * This is a simple implementation of a parallel reduce. Instead of reducing an array
 * of sums in the main function, it is reduced inside the threads.
 *
 * After each thread finishes its main job, it will move onto the reduction part.
 * There it will add its calculated value, with its neighbour. This is a for loop,
 * so each time the neiighbur changes. More on this on the code below.
*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUM 100

int size; // Array size
int * array; // The array
int thread_num; // Number of threads
int * gsum; // The reduce array

// Array initialization
void * array_init()
{
    int * array;

    array = (int *) malloc(size * sizeof(int));

    for (int i = 0; i < size; ++i)
    {
        array[i] = rand() % MAX_NUM;
    }

    return array;
}

// The parallel sum function
void par_sum_reduction (void)
{

    #pragma omp parallel
    {
        int rank = omp_get_thread_num();
        int my_sum = 0;
        int i;

        // Each thread calculates its local sum and assigns it at the 
        // gsum array (gsum[rank] = my_sum)
        // Mapping part
        #pragma omp for
            for (i = 0; i < size; ++i)
                my_sum += array[i];
        gsum[rank] = my_sum;

        // Now reduuce the gsum array
        #pragma omp barrier
        
        // This variable is something like a factor, that specifies which will be the task 
        // that will reduce the local sum of its neighbour. The neighbour's sum is specified
        // by the i variable (i is the distance, from the current task and the neighbour).
        // It is muultiiplied by 2 at each iteration, to reduce the number of tasks eligible
        // for performing reduction.
        int p2 = 2;
        for (i = 1; i <= thread_num; i *= 2) 
        {
            if ((rank % p2) == 0)
                if (rank + i < thread_num) 
                    gsum[rank] += gsum[rank + i]; // Left: this task, right: the neighbour


            p2 *= 2;

            // The barrier here ensures that all threads (meeting the constraints) will reduce 
            // their value with their neighbour, before doing that again. All in all, the threads
            // execute this block always with the same p2.
            #pragma omp barrier
        }

        /*
         * For example gsum = 3,5,6,2,7,3,6,2
         * 1st iter: p2 = 2, so eligible tasks: 0, 2, 4, 6 and neighbours 1, 3, 5, 7 accoordingly
         *           gsum = 8, 5, 8, 2, 10, 3, 8, 2
         * 2nd iter: p2 = 4, i = 2, eligible tasks: 0, 4 and neighbours 2, 6
         *           gsum = 16, 5, 8, 2, 13, 3, 6, 2
         * 3rd iter: p2 = 8, i = 4, eligible 0 and neighbour 4
         *           gsum = 29, ...
         * and sum total is 29.
         */
    }
}

int main (int argc, char * argv[])
{
    clock_t start, end;
    double total;

    if (argc < 3)
    {
        printf ("Usage: <%s> <thread_num> <array_siz>\n", argv[0]);
        exit (0);
    }

    thread_num = strtol (argv[1], NULL, 10);
    size = strtol (argv[2], NULL, 10);

    omp_set_num_threads(thread_num);
    omp_set_nested(1);
    array = array_init();
    gsum = (int *) calloc (thread_num, sizeof (int));

    pthread_t handles[thread_num];
    int ranks[thread_num];

    start = clock();
    par_sum_reduction();
    end = clock();

    total = ((double) end - start) / CLOCKS_PER_SEC;

    // Index 0 has the reduced sum
    // printf ("%d\n", gsum[0]);
    printf ("Time elapsed: %f\n", total);

    return EXIT_SUCCESS;
}
