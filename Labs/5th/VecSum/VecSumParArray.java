class VecSumParObj
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

		double sum = 0.0;
		for (int i = 0; i < numThreads; i++) {
			//System.out.println(tsum[i]);
			sum = sum + tsum[i];
		}

		// get current time and calculate elapsed time
		long elapsedTimeMillis = System.currentTimeMillis()-start;
		System.out.println("time in ms = "+ elapsedTimeMillis);

		System.out.println(sum);

	}
}

class VecSumThread extends Thread
{
	private double [] sums;
	private double [] table;
	private double mySum;
	private int myId;
	private int myStart;
	private int myStop;

	// constructor
	public VecSumThread(int id, int numThreads, double[] ts, int size, double[] array)
	{
		sums = ts;
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
		for(int i = myStart; i < myStop; i++) 
			//mySum = mySum + table[i];
			sums[myId] = sums[myId] + table[i];
		//sums[myId] = mySum	
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
