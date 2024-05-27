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
    double * my_sum; // Returned to main
    double step;
    int start;
    int end;
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
void * calc_pi(void * targs)
{
    struct targs * info = (struct targs *) targs;
    // This can also be local thrread variables.
    double x = 0.0;

    for (int i = info->start; i <= info->end; ++i)
    {
    	x = (info->step) * ((double)i - 0.5);
        (*(info->my_sum)) += f(x);
    }
}

int main( int argc, char *argv[])
{
    double total_time;
    double PI25DT = 3.141592653589793238462643;
    double pi, step, sum;
    double *local_sum;
    int i;
    int th_num;
   
    if (argc != 3) {
		printf ("Usage : %s <number_of_intervals> <th_num>\n", argv[0]);
		return 1;
    }
    
    th_num = strtol(argv[2], NULL, 10);

    double * th_sums = (double *) calloc(th_num, sizeof(double));
    pthread_t tid[th_num];
    struct targs targs[th_num];
    long int n = strtol(argv[1], NULL, 10);
    
    sum = 0.0;
    pi = 0.0;
    step  = 1.0 / (double) n;

    total_time = get_time(); 
    
    for (i = 0; i < th_num; ++i)
    {
        targs[i].my_sum = &th_sums[i];
        targs[i].step = step;
        
        // Calculate starr and end index for eeach thrread
        // I think it slows the program a little biiit
        targs[i].start = (n / th_num) * i + 1;
        targs[i].end = (n / th_num) + targs[i].start - 1;
        if (i == th_num - 1)
            targs[i].end = n;

        printf("%d, %d, %d\n", i, targs[i].start, targs[i].end);

        pthread_create(&tid[i], NULL, calc_pi, &targs[i]);
    }

    for (i = 0; i < th_num; ++i)
    {
        // After the end of each thread, get its calcuulated sum
        pthread_join(tid[i], NULL);
        sum += th_sums[i];
    }

    // Now get pi
    pi = sum * step;

    total_time = get_time() - total_time; 
    
    printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    printf("Total time (s): %f\n", total_time);

        
   return 0;
}

            
