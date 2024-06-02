# Results
All the tests were made with 100000000 size and 4 threads and using the pthread library to start the threads.

## Static
The for loop iterations are statically distributed. There are no locks needed in this implementation. The total time taken for the main algorithm to finish was around 1.95 seconds.

## Cyclic
Something similar like the static, except that in this case, thee start index of each thead is `my_rank` and their next index `my_rank + th_num`. This took around 1.27 seconds to finish.

## Dynamic
The last one uses locks to implement its logic. A thread dynamically asks for an index of the array to be given to it. The total time taken for the main sieve algorithm to finish was around 4 seconds.

## Results
The Dynamic one is slower, as it uses locking mechanisms for every iteration of the outter loop. The other two achieve better results, because they do not use locking mechanisms, while the cyclic algorithm may be slightly better than the static, maybe becuase the static one makes divisions with very large numbers (which can be solved using shifts, if and only if we know that the threads are multiples of 2).
