The sequential algorithm needs around 0.615 seconds, while the parallel one, using the openmp library, just needs around 0.22 seconds. The time is reduced by 3 times.

The parallel algorithm reduces the total_matches value and divides the reader buffer to the created threads.

A file containing the E.coli fiile 20 times was created and was tested for 4 threads and the pattern that was provided “tacccagattatcgccatcaacgg” (it exists only 20 times).
