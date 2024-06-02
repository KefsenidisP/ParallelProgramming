# Test
All the tests were made with number of intervals equal to 100000000.

# Results
I found out that blocks from size 100000 to size 10000000, for this particular number of intervals, results in the best possible times (around 0.25 seconds each). This may be because there must be a perfect balance between the number of threads and the amount of workload that is being assigned to each thread and the increasion and deduction of the two values accordingly results in no changes.

# In comparisson to the ones from lab 5
I run the codes from the previous labs some times, each time, with different number of threads

## pi_par_main.c
Found out, that this one took around the same time as this algorithm to finish only when we run it with 128 threads. When run with 256 threads, the time needed to finish was increased.

## pi_locks.c
Tessted this for up to 4096 threads. This uses locks, so the increasion of the number of threads, is expected to cause an increasion to the time needed by the algorithm to finish running and that is what happens. It had far worse performance than this algorithm (starting from 5.3 seconds and reaching up to 6.3 seconds).

## pi_sync.c
This was like the previous algorithm, but with far worse elapsed time.

## pi_poor_locks.c
When tested with 4 threads the elapsed time was around 1 second, but when the number of threads, was increased to 32, the elapsed time increased to around 8 seconds.

# Summary
The recursive algorithm created for this lab is better than the algorithms created for the previous lab

# Note
Do not try for blocks with size less than 1000, for really big sizes, becaue it will just cause a very slow segmentation fault.
