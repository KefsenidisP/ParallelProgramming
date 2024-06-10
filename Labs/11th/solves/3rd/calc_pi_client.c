/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "calc_pi.h"
#include <pthread.h>
#include <stdlib.h>

struct thread_args_t {
    int thread_num;
    groupid_t groupid;
    CLIENT * clnt;
};

pthread_mutex_t assign_mutex = PTHREAD_MUTEX_INITIALIZER;

double f( double a )
{
    return (4.0 / (1.0 + a * a));
}

double sum_calculation (worker_args_t wargs)
{
    int start;
    int end;
    double sum;
    double step;
    double x;

    sum = 0.0;
    x = 0.0;
    step = 1.0 / wargs.iters_max;
    start = wargs.rank * wargs.thread_iters + 1;
    end = start + wargs.thread_iters - 1;

    for (int i = start; i <= end; ++i)
    {
        x = step * ((double)i - 0.5);
        sum += f(x);
    }
    printf("%d, %d, %d\n", start, end, wargs.thread_iters);

    return sum;
}

// The thread function
void * worker_thread (void * thread_args)
{
    struct thread_args_t * targs = (struct thread_args_t *) thread_args;
    worker_args_t * wargs;
    master_args_t margs;

    margs.thread_num = targs->thread_num;
    margs.groupid = targs->groupid;

    // Get workload from server
    pthread_mutex_lock(&assign_mutex);
    wargs = assign_to_task_1 (&margs, targs->clnt);
    pthread_mutex_unlock(&assign_mutex);
    // Calculate local sum
    margs.sum = sum_calculation(*wargs);
    // And send the calculated local sum to the master
    send_sum_1(&margs, targs->clnt);

    // And terminate thread.
    return NULL;
}

void
mas_wor_1 (char *host, int thread_num)
{
	CLIENT * clnt;
    pthread_t tid[thread_num];
    struct thread_args_t targs[thread_num];
    groupid_t * groupid;

#ifndef	DEBUG
	clnt = clnt_create (host, MAS_WOR, MAS_WOR_V_1, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

    // First inform the master that a new group has been created 
    // and get a group id.
    groupid = get_groupid_1(NULL, clnt);

    if (*groupid == -1)
    {
        puts ("Master busy...");
        exit (EXIT_SUCCESS);
    }

    for (int i = 0; i < thread_num; ++i)
    {
        targs[i].thread_num = thread_num;
        targs[i].groupid = *groupid;
        targs[i].clnt = clnt;

        pthread_create (&tid[i], NULL, worker_thread, &targs[i]);
    }

    for (int i = 0; i < thread_num; ++i)
        pthread_join (tid[i], NULL);

    // And now let the master know that this group has finished calculation
    mark_as_finished_1(groupid, clnt);

#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int 
main (int argc, char *argv[])
{
	char * host;
    int thread_num;

	if (argc < 3) {
		printf ("usage: %s <server_host> <thread_num>\n", argv[0]);
		exit (1);
	}
	
    host = argv[1];
    thread_num = strtol(argv[2], NULL, 10);

	mas_wor_1 (host, thread_num);
    
    exit (0);
}
