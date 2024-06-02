/*
 * The main function uses the threads to calculate a part of the sum
 * variable, then it "reduces" it and calculatees pi.
 *
 * It seems mooree accurate than the sequential for bigger values
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

struct targs {
    double * sum; // Returned to main
    double step;
    int start;
    int end;
    int limit; // At which point to stop creating threads
};

double get_time () {
	struct timeval t;    
    gettimeofday(&t, NULL); 
    return (t.tv_sec + t.tv_usec / 1000000.0);
}

double f( double a )
{
    return (4.0 / (1.0 + a * a));
}

// The threads just calculate their own sum and return it to main
void calc_pi(void * targs)
{
    struct targs * info = (struct targs *) targs;
    // This can also be local thrread variables.
    double x = 0.0;

    for (int i = info->start; i <= info->end; ++i)
    {
    	x = (info->step) * ((double)i - 0.5);
        (*(info->sum)) += f(x);
    }
}

void * work_assign(void * targs)
{
    pthread_t tid;
    struct targs * tmp = (struct targs *) targs;
    int workload; // The work load that is assigned to a thread

    workload = (tmp->end) - (tmp->start);

    if (workload <= (tmp->limit))
        calc_pi(targs);
    else 
    {
        struct targs new_thread = *(tmp);
        struct targs this_thread = *(tmp);
        double new_sum = 0.0;
        double this_sum = 0.0;
        int mid;

        // Cut the array in half
        mid = (tmp->start) + workload / 2;
        
        // Change start and end values of the arguments (and some other values)
        new_thread.start = mid;
        new_thread.sum = &new_sum;

        this_thread.end = mid - 1;
        this_thread.sum = &this_sum;

        // Create a new thread and assign one half to it
        pthread_create(&tid, NULL, work_assign, (void *) &new_thread);

        // And call this function again for this thread
        work_assign((void *) &this_thread);

        // Wait for the thead to finish
        pthread_join(tid, NULL);

        // Now add the this_sum and new_sum to targs->sum and return
        (*(tmp->sum)) = new_sum + this_sum;
    }
}

int main( int argc, char *argv[])
{
    double total_time;
    double PI25DT = 3.141592653589793238462643;
    double pi, step, sum;
    double *local_sum;
    int i;
   
    if (argc != 3) {
		printf ("Usage : %s <number_of_intervals> <limit>\n", argv[0]);
		return 1;
    }
    
    pthread_t tid;
    struct targs targs;
    long int n = strtol(argv[1], NULL, 10);
    int limit = strtol(argv[2], NULL, 10);

    sum = 0.0;
    pi = 0.0;
    step  = 1.0 / (double) n;

    total_time = get_time(); 
    
    targs.sum = &sum;
    targs.step = step;
    targs.start = 1;
    targs.end = n;
    targs.limit = limit;

    pthread_create(&tid, NULL, work_assign, &targs);

    // After the end of each thread, get its calcuulated sum
    pthread_join(tid, NULL);
    printf("Sum: %.16f\n", sum);
    // Now get pi
    pi = sum * step;
    printf("Sum: %.16f\n", pi);

    total_time = get_time() - total_time; 
    
    printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    printf("Total time (s): %f\n", total_time);

        
   return 0;
}

            
