class CyclicSimple
{
	public static void main(String[] args)
	{
        int size = 100000;
        int numThreads = Runtime.getRuntime().availableProcessors();
                
		double[] a = new double[size];
		for(int i = 0; i < size; i++)
			a[i] = i; 
  
        // get current time
		long start = System.currentTimeMillis();
		
		SqrtGroupThread threads[] = new SqrtGroupThread[numThreads];
		
		for(int i = 0; i < numThreads; i++) 
		{
			threads[i] = new SqrtGroupThread(a, i, numThreads, size);
			threads[i].start();
		}
	   
		// wait for threads to terminate            
		for(int i = 0; i < numThreads; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {}
		}

		// get current time and calculate elapsed time
		long elapsedTimeMillis = System.currentTimeMillis()-start;
		System.out.println("time in ms = "+ elapsedTimeMillis);

	}
}

class SqrtGroupThread extends Thread
{
	private double [] table;
	private int myrank;
	private int numThreads;
	private int size;
	

	public SqrtGroupThread(double [] array, int rank, int nthreads, int s)
	{
		table = array;
		myrank = rank;
		numThreads = nthreads;
		size = s;
	}

	
	public void run()
	{
		for(int i = myrank; i < size; i += numThreads) {
			System.out.println(myrank+" index " + i);
			table[i] = Math.sqrt(table[i]);
		}	
	}
}
