
public class SimpleRecursive {

    public static void main(String[] args) {
    
	int size = 100000;
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

	/* create and start thread 0 */

	Recursive mythread = new Recursive(0, size, limit, a);
	mythread.start();        

	/* wait for thread 0 */
	int sum = 0;
	try {
		mythread.join();
		sum = mythread.myResult;
	} catch (InterruptedException e) {}

	System.out.println("Sum "+sum);
    }
}

class Recursive extends Thread {

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
			//System.out.printf("L %d %d %d \n",myFrom, mid, myLimit);
			Recursive threadL = new Recursive(myFrom, mid, myLimit, myA);
			threadL.start();
			//System.out.printf("R %d %d %d \n", mid, myTo, myLimit);
			Recursive threadR = new Recursive(mid, myTo, myLimit, myA);
			threadR.start();
			try {
				threadL.join();
				myResult = threadL.myResult;
				threadR.join();
				myResult += threadR.myResult;
			} catch (InterruptedException e) {}
	    }
   }

}

