/*
 * Cyclic barrier with the cuustm synchronization statetment
 *
 * I believe it will look more like the solution with locks and 
 * condition variables 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SLEEP 50000
#define synchronized(obj) \
    while (pthread_mutex_trylock(&obj)); \
    for (int locked = 1; locked; locked = pthread_mutex_unlock(&obj))

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
    // Locked, increase
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
    pthread_cond_signal(args->inside);

    // A thread awakens and catches the lock blocking other threads to continue
    // (I think saw this implemented somewhere in the scheduler)
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
    
    // Wake them up (final thread) 
    pthread_cond_signal(args->leaving);
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
        synchronized(*(args->mutex)) cyclic_barrier(args);
        printf("Thread %d has started!\n", args->rank);

        synchronized(*(args->mutex)) cyclic_barrier(args);
        printf("Thread %d has reached the barrier\n", args->rank);
        synchronized(*(args->mutex)) cyclic_barrier(args);
        printf("Thread %d has passed the barrier\n", args->rank);
        synchronized(*(args->mutex)) cyclic_barrier(args);

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
