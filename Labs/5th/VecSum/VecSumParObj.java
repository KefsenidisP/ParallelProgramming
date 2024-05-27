class VecSumParObj
{
	public static void main(String[] args)
	{
        int size = 100000;
        int numThreads = Runtime.getRuntime().availableProcessors();

		double[] a = new double[size];
		for(int i = 0; i < size; i++)
			a[i] = i;
     		
		sumObj sum = new sumObj();

		// get current time
		long start = System.currentTimeMillis();

		// create threads
		VecSumThread threads[] = new VecSumThread[numThreads];
		
		// thread execution   
		for (int i = 0; i < numThreads; i++) 
		{
			threads[i] = new VecSumThread(i, numThreads, size, a, sum);
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

		sum.printout();

	}
}

class VecSumThread extends Thread
{
	private double [] table;
	private double mySum;
	private sumObj globalSum;
	private int myId;
	private int myStart;
	private int myStop;

	// constructor
	public VecSumThread(int id, int numThreads, int size, double[] array, sumObj s)
	{
		table = array;
		mySum = 0.0;
		globalSum = s;
		myId = id;
		myStart = myId * (size / numThreads);
		myStop = myStart + (size / numThreads);
		if (myId == (numThreads - 1)) myStop = size;
	}

	// thread code
	public void run()
	{
		for(int i = myStart; i < myStop; i++)
			mySum = mySum + table[i];
			
		globalSum.add(mySum);  
	}

}

class sumObj {

    double sum;
    
    public sumObj (){
		this.sum = 0;
    }

    public synchronized void add (double localsum){
		this.sum = this.sum + localsum;
    }

    public synchronized void printout() {
		System.out.println(this.sum);
    }
}
