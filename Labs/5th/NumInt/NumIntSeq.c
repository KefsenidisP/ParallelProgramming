# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <math.h>
#include <sys/time.h>


double get_time ();
double f( double );


double get_time () {
	struct timeval t;
        
        gettimeofday(&t, NULL); 
        return (t.tv_sec + t.tv_usec/1000000.0);
}


double f( double a )
{
    return (4.0 / (1.0 + a*a));
}

int main( int argc, char *argv[])
{
    double total_time;
    
    double PI25DT = 3.141592653589793238462643;
    double pi, h, sum, x;
    double *local_sum;
    int i;

   
    if (argc != 2) {
		printf ("Usage : %s <number_of_intervals>\n", argv[0]);
		return 1;
    }
    
    long int n = strtol(argv[1], NULL, 10);
    pi = 0.0;
    h  = 1.0 / (double) n;
    sum = 0.0;

    total_time = get_time(); 

    for (i = 1; i <= n; i++)
    {
    	x = h * ((double)i - 0.5);
        sum += f(x);
    }
    printf("%f\n", sum);
    pi = h * sum;

    total_time = get_time() - total_time; 
    printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    printf("Total time (s): %f\n", total_time);

        
   return 0;
}

            
