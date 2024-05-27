import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

class VecSumParTree
{
	public static void main(String[] args)
	{
        int size = 100000;
        int numThreads = Runtime.getRuntime().availableProcessors();

		double[] a = new double[size];
		for(int i = 0; i < size; i++)
			a[i] = i;
     		
		double[] tsum = new double[numThreads];
		for(int i = 0; i < numThreads; i++)
			tsum[i] = 0.0;
     
		// get current time
		long start = System.currentTimeMillis();

		// create threads
		VecSumThread threads[] = new VecSumThread[numThreads];
		
		// thread execution   
		for (int i = 0; i < numThreads; i++) 
		{
			threads[i] = new VecSumThread(i, numThreads, tsum, size, a);
			threads[i].start();
		}

		// wait for threads to terminate            
		for (int i = 0; i < numThreads; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {}
		}

		// get current time and calculate elapsed time
		long elapsedTimeMillis = System.currentTimeMillis()-start;
		System.out.println("time in ms = "+ elapsedTimeMillis);

		System.out.println(tsum[0]);

	}
}

class VecSumThread extends Thread
{
	private double [] sums;
	private double [] table;
	private double mySum;
	private int myId;
	private int myNumThreads;
	private int myStart;
	private int myStop;
	// barrier
	static CyclicBarrier barrier; 
	
	// constructor
	public VecSumThread(int id, int numThreads, double[] ts, int size, double[] array)
	{
		sums = ts;
		table = array;
		mySum = 0.0;
		myId = id;
		myNumThreads = numThreads;
		myStart = myId * (size / myNumThreads);
		myStop = myStart + (size / myNumThreads);
		if (myId == (myNumThreads - 1)) myStop = size;
		// barrier
		barrier = new CyclicBarrier(myNumThreads);
	}

	// thread code
	public void run()
	{
		for(int i = myStart; i < myStop; i++) 
			mySum = mySum + table[i];
		sums[myId] = mySum;	
		reduction_tree();
	}
	
	void reduction_tree()
	{
		int tree_depth = (int)(Math.log(myNumThreads) / Math.log(2));
		
		for (int j=1; j<=tree_depth; j++) {
		    // barrier wait
		    try {

                barrier.await();
            } catch (InterruptedException | BrokenBarrierException e) {
                e.printStackTrace();
            }
            
			if ((myId % (int)Math.pow(2, j)) == 0) {
				sums[myId] += sums[myId + (int)Math.pow(2, j-1)];
				System.out.println("j ="+j+" myId = "+myId+" sums ="+sums[myId]);
			}
	   }			
	}			
}
