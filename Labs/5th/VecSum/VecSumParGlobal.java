import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

class VecSumParGlobal
{
	public static double sum;
	public static Lock lock = new ReentrantLock();

	public static void main(String[] args)
	{
        int size = 100000;
        int numThreads = Runtime.getRuntime().availableProcessors();

		double[] a = new double[size];
		for(int i = 0; i < size; i++)
			a[i] = i;
		
		sum = 0.0;

		// get current time
		long start = System.currentTimeMillis();

		// create threads
		VecSumThread threads[] = new VecSumThread[numThreads];
		
		// thread execution   
		for (int i = 0; i < numThreads; i++) 
		{
			threads[i] = new VecSumThread(i, numThreads, size, a);
			threads[i].start();
		}

		// wait for threads to terminate and collect result
                 
		for (int i = 0; i < numThreads; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {}
		}
		
		// get current time and calculate elapsed time
		long elapsedTimeMillis = System.currentTimeMillis()-start;
		System.out.println("time in ms = "+ elapsedTimeMillis);

		System.out.println(sum);

	}
}

class VecSumThread extends Thread
{
	private double [] table;
	private double mySum;
	private int myId;
	private int myStart;
	private int myStop;

	// constructor
	public VecSumThread(int id, int numThreads, int size, double[] array)
	{
		table = array;
		myId = id;
		myStart = myId * (size / numThreads);
		myStop = myStart + (size / numThreads);
		if (myId == (numThreads - 1)) myStop = size;
		//mySum = 0.0;
	}

	// thread code
	public void run()
	{
	    for(int i = myStart; i < myStop; i++) {
		    VecSumParGlobal.lock.lock();
			try {
			     VecSumParGlobal.sum = VecSumParGlobal.sum + table[i];
			} finally {
			     VecSumParGlobal.lock.unlock();
			}
		}
		/*for(int i = myStart; i < myStop; i++)
			mySum = mySum + table[i];
			
		VecSumParGlobal.lock.lock();
		try {
			VecSumParGlobal.sum = VecSumParGlobal.sum + mySum;
		} finally {
			VecSumParGlobal.lock.unlock();
		}*/
	}

}
