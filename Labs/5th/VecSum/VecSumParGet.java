class VecSumParGet
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

		// create threads
		VecSumThread threads[] = new VecSumThread[numThreads];
		
		// thread execution   
		for (int i = 0; i < numThreads; i++) 
		{
			threads[i] = new VecSumThread(i, numThreads, size, a);
			threads[i].start();
		}

		// wait for threads to terminate and collect result
        
        double sum = 0.0;         
		for (int i = 0; i < numThreads; i++) {
			try {
				threads[i].join();
				sum = sum + threads[i].get();
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
	public double mySum;
	private int myId;
	private int myStart;
	private int myStop;

	// constructor
	public VecSumThread(int id, int numThreads, int size, double[] array)
	{
		table = array;
		mySum = 0.0;
		myId = id;
		myStart = myId * (size / numThreads);
		myStop = myStart + (size / numThreads);
		if (myId == (numThreads - 1)) myStop = size;
	}
	
	public double get() 
	{
		return mySum;
	}   

	// thread code
	public void run()
	{
		for(int i = myStart; i < myStop; i++) 
           	mySum = mySum + table[i];
	}
}