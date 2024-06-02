/*
 * Cyclic barrier with flags and locks
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SLEEP 50000

// Need one lock variable and two conditions
struct thread_args {
    int rank; // The thread's rank
    int threads; // Number of threads
    pthread_mutex_t * mutex; // The lock
    pthread_cond_t * inside; // Condition when thread is finaly inside the function
    pthread_cond_t * leaving; // Condition when thread is leaving function
};

int arrived = 0; // How many threads have arrived at the barrier
int flag_leaving = 0;
int flag_inside = 0;

void cyclic_barrier(struct thread_args * args)
{

    // So the thread must lock the mutex at first, to increase the arraived global
    pthread_mutex_lock(args->mutex);
    arrived++;
    
    // Set the flags when the last thread enters here
    if (arrived == args->threads)
    {
        flag_leaving = 0;
        flag_inside = 1;
    }

    // All threads wait here and unlock mutex on condition
    while (!flag_inside)
        pthread_cond_wait(args->inside, args->mutex);

    // The final thrtead must wake up all the other threads on the condition
    // and of course unlock the lock
    pthread_cond_signal(args->inside);
    pthread_mutex_unlock(args->mutex);

    // Now acquire the lock again, this time to decrease the arraived threads,
    // as they will now be leaving from the function
    pthread_mutex_lock(args->mutex);
    arrived--;

    // Reset the flags
    if (arrived == 0)
    {
        flag_leaving = 1; // Allow threads to leave
        flag_inside = 0; // Block any thread to come inside again
    }
    
    // Wait on condition 
    while (!flag_leaving)
        pthread_cond_wait(args->leaving, args->mutex);
    
    // Wake them up (final thread) and unlock mutex
    pthread_cond_signal(args->leaving);
    pthread_mutex_unlock(args->mutex);
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
    pthread_cond_t leaving, inside;
    pthread_mutex_t mutex;
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

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&inside, NULL);
    pthread_cond_init(&leaving, NULL);

    for (int i = 0; i < threads; ++i)
    {
        args[i].rank = i;
        args[i].threads = threads;
        args[i].mutex = &mutex;
        args[i].inside = &inside;
        args[i].leaving = &leaving;

        pthread_create(&tid[i], NULL, func_par, &args[i]);
    }

    for (int i = 0; i < threads; ++i)
        pthread_join(tid[i], NULL);

    return EXIT_SUCCESS;
}
