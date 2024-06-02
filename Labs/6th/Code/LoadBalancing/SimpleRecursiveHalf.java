
public class SimpleRecursiveHalf {

    public static void main(String[] args) {

	int size = 1000000;
	int limit = 128;
	
	/*  
	if (args.length != 1) {
		System.out.println("Usage: java SimpleRecursive <vector size>");
		System.exit(1);
	}

	try {
		size = Integer.parseInt(args[0]);
	}
	catch (NumberFormatException nfe) {
   		System.out.println("Integer argument expected");
		System.exit(1);
	}
	if (size <= 0) {
		System.out.println("size should be positive integer");
		System.exit(1);
	}
    */
    
	int[] a = new int[size];
	for(int i = 0; i < size; i++)
		a[i] = i; 

	/* create recusrive function and start thread 0 */
	Recursive mytask = new Recursive(0, size, limit, a);
	Thread mythread = new Thread(mytask); 
	mythread.start();        

	/* wait for thread 0 */
	int sum = 0;
	try {
		mythread.join();
		sum = mytask.myResult;
	} catch (InterruptedException e) {}

	System.out.println("Sum "+sum);
    }
}

class Recursive implements Runnable {

	private int myFrom;
	private int myTo;
	private int myLimit;
	private int[] myA;
	public  int myResult;
   
	public Recursive (int from, int to, int limit, int[] a) {
		this.myFrom = from;
		this.myTo = to;
		this.myLimit = limit;
		this.myA = a;
		this.myResult = 0;
	}

	public void run() {
		/* do recursion until limit is reached */
		System.out.println("In thread"+Thread.currentThread().getName());
		int workload = myTo - myFrom;
		if (workload <= myLimit) {
		    //System.out.printf("Cutoff %d %d \n",myFrom, myTo);
			myResult = 0;
        	for (int i=myFrom; i<myTo; i++)
        		myResult += myA[i];
		} else {
			int mid = myFrom + workload / 2;
			/* create threads only on left recursion branches */
			//System.out.printf("new taskL->threadL %d %d \n",myFrom, mid);
			Recursive taskL = new Recursive(myFrom, mid, myLimit, myA);
			Thread threadL = new Thread(taskL); 
			threadL.start();
			//System.out.printf("new taskR %d %d \n", mid, myTo);
			/* right recursion branches are executed in current thread */
			Recursive taskR = new Recursive(mid, myTo, myLimit, myA);
			taskR.run();
			try {
				threadL.join();
				myResult = taskL.myResult + taskR.myResult;
			} catch (InterruptedException e) {}
	    }
   }

}

