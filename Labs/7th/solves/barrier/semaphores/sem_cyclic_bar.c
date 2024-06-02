/*
 * Cyclic barrier with semaphores
 *
 * Unlike the one with the locks the while loops are not neeeded
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SLEEP 50000

// Need three semaphores: 
// 1) to block the arrived variable increasion
// 2) to block the threads inside the function barried
// 3) to block the threads at the return point
struct thread_args {
    int rank; // The thread's rank
    int threads; // Number of threads
    sem_t * sem; // The semaphore for locking the arraived increasion
    sem_t * inside; // Condition when thread is finaly inside the function
    sem_t * leaving; // Condition when thread is leaving function
};

int arrived = 0; // How many threads have arrived at the barrier
int flag_leaving = 0;
int flag_inside = 0;

void cyclic_barrier(struct thread_args * args)
{

    // Reduce the value of the mutex to 0
    sem_wait(args->sem);
    arrived++;
    
    // Set the valuue of the semaphores
    if (arrived == args->threads)
    {
        sem_post(args->inside); // Unlock the inner parts of the fuuunction
        sem_wait(args->leaving); // Lock the exit
    }

    // Unlock the sem semaphore
    sem_post(args->sem);

    // And wait for the other threads to reach this part
    sem_wait(args->inside);
    sem_post(args->inside);

    // Acquire the semaphore again
    sem_wait(args->sem);
    arrived--;

    // And reset the semaphores to their initial values
    if (arrived == 0)
    {
        sem_wait(args->inside); // Lock the entrance
        sem_post(args->leaving); // Allow exiting
    }
    
    // Unlock semaphore
    sem_post(args->sem);

    // And wait for other threads to reach this point 
    sem_wait(args->leaving);
    sem_post(args->leaving);
}

/*
 * The messages are printed in order (not rank order),
 * so I believe it is correct
*/
// Just a simple test function
void * func_par(void * thread_args)
{
    // Parse it to the real type
    struct thread_args * args = (struct thread_args *) thread_args;

    while (1)
    {
        cyclic_barrier(args);
        printf("Thread %d has started!\n", args->rank);

        cyclic_barrier(args);
        printf("Thread %d has reached the barrier\n", args->rank);
        cyclic_barrier(args);
        printf("Thread %d has passed the barrier\n", args->rank);
        cyclic_barrier(args);

        usleep(SLEEP);
    }
}

int main(int argc, char * argv[])
{
    sem_t leaving, inside, sem;
    pthread_t * tid;
    struct thread_args * args;
    int threads;

    if (argc != 2)
    {
        printf("Usage: <%s> <threads_num>\n", argv[0]);
        exit(1);
    }

    // Some initializations
    threads = strtol(argv[1], NULL, 10);
    
    args = (struct thread_args *) malloc(threads * sizeof(struct thread_args));
    tid = (pthread_t *) malloc(threads * sizeof(pthread_t));

    sem_init(&sem, 0, 1); // Locked
    sem_init(&inside, 0, 0); // Locke
    sem_init(&leaving, 0, 1); // Not locked

    for (int i = 0; i < threads; ++i)
    {
        args[i].rank = i;
        args[i].threads = threads;
        args[i].sem = &sem;
        args[i].inside = &inside;
        args[i].leaving = &leaving;

        pthread_create(&tid[i], NULL, func_par, &args[i]);
    }

    for (int i = 0; i < threads; ++i)
        pthread_join(tid[i], NULL);

    return EXIT_SUCCESS;
}
