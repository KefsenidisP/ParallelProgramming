import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class GlobalVarsLock {

    static int n = 0;
    static Lock lock = new ReentrantLock();
    
    public static void main(String[] args) {

        int numThreads = 4;  
		CounterThread counterThreads[] = new CounterThread[numThreads];
		for (int i = 0; i < numThreads; i++) {
		    counterThreads[i] = new CounterThread();
		    counterThreads[i].start();
		}

        for (int i = 0; i < numThreads; i++) {
            try {
		        counterThreads[i].join();
            }
            catch (InterruptedException e) {}
		} 
	    System.out.println("n = "+n); 
    }

    static class CounterThread extends Thread {
	  	
	public void run() {

	    for (int i = 0; i < 10000; i++) {
	        lock.lock();
			try { 
			    n = n + 1;
		    } finally {
				lock.unlock() ;
			}
	        }	
		}
    }
}	
	
