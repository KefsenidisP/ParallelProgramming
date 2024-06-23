/*
 * This is an example implementation of a map reduce, in parallel prrogramming.
 *
 * The user provides 2 numbers start and end of a range. The algorithm checks
 * if the sum of all the divisors of a number, (plus the number) within the range,
 * divided by this number (called abundancy ratio) is the equal to the sum of another 
 * number within the range also divided by this number (refering to another number). 
 * If this is true (the abundancy ratio of two different numbers is equal) then two 
 * numbers can be considered friendly
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int gcd(int u, int v)
{
    if (v == 0) 
        return u;
    return gcd(v, u % v);
}

void friendly_numbers (int start, int end)
{
    int range_size = end - start + 1; // Size of the range
    int * range = (int *) malloc (range_size * sizeof (int)); // The range of numbers
    int * num = (int *) malloc (range_size * sizeof (int)); // The divisor sum plus the number (index is corresponding number in range)
    int * den = (int *) malloc (range_size * sizeof (int)); // The abundancy ratio divisor of the number
    
    #pragma omp parallel
    {
        int i, j, ii;
        // i: the number in the range to compute abundancy ratio (and indexing in scond loop)
        // j: just for indexing
        // ii: index of the number in the range

        int factor; // A divisor of the number
        int sum; // tmp varible, for num[idx] (where idx the index of the number in the range)
        int done; // Number to invstigate (for divisors)
        int n; // gcd return val

        // This is the mapping section, where abundancy ratios are calculated
        #pragma omp for schedule (dynamic, 16)
            for (i = start; i <= end; ++i)
            {
                ii = i - start; // Correspond num/den index, to number in range
                sum = 1 + i;
                range[ii] = i;
                done = i;
                factor = 2;

                // Check f divisor and add it in the sum variable
                while (factor < done) 
                {
                    if ((i % factor) == 0) 
                    {
                        sum += (factor + (i/factor));
                        if ((done = i/factor) == factor) 
                            sum -= factor;
                    }
                    factor++;
                }

                num[ii] = sum; 
                den[ii] = i;

                // Use gcd to make division computations easier, since we use integers and not floats
                n = gcd(num[ii], den[ii]);
                num[ii] /= n;
                den[ii] /= n;
                // abundancy ratio = num[ii] / den[ii]
                // So two nuumbers are frieedly if and only if num[i] == num[j] and 
                // den[i] == den[j]
            }

        // reduction part, where the checking oof frieedly numbers is done
        #pragma omp for schedule (static, 8)
            for (i = 0; i < range_size; i++) 
                for (j = i + 1; j < range_size; j++)
                    if ((num[i] == num[j]) && (den[i] == den[j]))
                        continue;
                        // printf ("%d and %d are FRIENDLY \n", range[i], range[j]);
    }
}

int main (int argc, char * argv[])
{
    int thread_num;
    int start;
    int end;
    clock_t time_clock;
    double time;

    if (argc != 4)
    {
        printf ("Usage: <%s> <start> <end> <thread_num>\n", argv[0]);
        exit (0);
    }

    start = strtol (argv[1], NULL, 10);
    end = strtol (argv[2], NULL, 10);
    thread_num = strtol (argv[3], NULL, 10);

    omp_set_num_threads (thread_num);
    omp_set_nested (1);

    time_clock = clock();
    friendly_numbers (start, end);
    time_clock = clock() - time_clock;

    time = ((double) time_clock) / CLOCKS_PER_SEC;

    printf ("Time elapsed: %f\n", time);

    return EXIT_SUCCESS;
}
