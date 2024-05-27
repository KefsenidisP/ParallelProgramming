// Something between lock and synchronized
  
public class MainArgsLockObj {

    public static void main(String[] args) {

        int numThreads = 4;  

        SharedCounter counter = new SharedCounter();
	
		CounterThread counterThreads[] = new CounterThread[numThreads];
		for (int i = 0; i < numThreads; i++) {
		    counterThreads[i] = new CounterThread(counter);
		    counterThreads[i].start();
		}
	
        for (int i = 0; i < numThreads; i++) {
            try {
	        counterThreads[i].join();
            }
            catch (InterruptedException e) {}
		} 
        System.out.println("n = "+counter.get());
    }

}

class SharedCounter {

    int n;
    Object lockObj = new Object();
  
    public SharedCounter(){
	    this.n = 0;
    }
    
    public int get() {
		synchronized (lockObj) {
	       	return n;
	    }   	
    }

    public void inc() {
		synchronized (lockObj) {
			n = n + 1;
        }
    }

}


class CounterThread extends Thread {
  	
    SharedCounter count;
    
    public CounterThread(SharedCounter counter) {
        this.count = counter;
    }
  	
    public void run() {
    
        for (int i = 0; i < 10000; i++) 
            count.inc();
    }    
}
