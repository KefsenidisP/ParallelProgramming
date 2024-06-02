import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MasterWorkerTaskLock {
	
	static int totalData; // μεγεθος προβλήματος
	static int totalTasks; // αριθμός εργασιών
	static int nThreads; // αριθμός νημάτων  
	static int tasksAssigned = -1; // μοιραζόμενη μεταβλητή μετρητή εργασιών   
	static Lock lock = new ReentrantLock();

	public static void main(String[] args)
	{
	    totalData = 200;
		totalTasks = 20; 
		nThreads = 10; 
		
		// Δημιουργία και αρχικοποίηση πίνακα    
		double[] a = new double[totalData]; 
		for(int i=0; i<totalData; i++)
		{
			a[i] = i + 3;
			//System.out.println(a[i]);   
		} 
		
		// Δημιουργία εργασιών
		Runnable[] tasks = new Runnable[totalTasks]; 
		int blockSize = totalData/totalTasks;
		for(int i=0; i<totalTasks; i++)
		{
		    int start = i*blockSize;
		    int stop = start+blockSize;
		    if (i == totalTasks-1) stop = totalData;
			tasks[i] = new myTask(a, i, start, stop);   
		} 

		// Δημιουργία νημάτων εργαζομένων
		Thread[] threads = new Thread[nThreads];
		for (int i = 0; i < threads.length; ++i)
		{
			threads[i] = new Worker(tasks);
		}		

		// Εκκίνηση νημάτων εργαζομένων 
		for (int i = 0; i < threads.length; ++i)
		{
			threads[i].start();
		}

		// Αναμονή τερματισμού νημάτων εργαζομένων 
		for (int i = 0; i < threads.length; ++i)
		{
			try {
				threads[i].join();
			}
			catch (InterruptedException e) {
				System.err.println("this should not happen");
			}
		}
		
		// Εμφάνιση στοιχείων πίνακα
		for(int i = 0; i < totalData; i++)
			System.out.println(a[i]);
	}

	//  Κρίσιμo τμήμα για την διανομή εργασιών
	private static int getTask()
	{
        lock.lock();
        try {
			// Διανέμει μια εργασία (στοιχείο πίνακα) 
			if (++tasksAssigned < totalTasks) 
				return tasksAssigned;
			else
				return -1;
        } finally {
        	lock.unlock() ;
        }			
	}


    private static class myTask implements Runnable
    {
	   private double[] table;
	   private int myID;
	   private int myStart;
	   private int myStop;
	   
	   public myTask(double[] array, int id, int start, int stop)
	   {
			table = array;
			this.myID = id;
			this.myStart = start;
			this.myStop = stop;
	   }
	   
	   public void run() 
	   {
	       for (int i=myStart; i<myStop; i++)
	           table[i]= Math.sqrt(table[i]);
	   }
	   //System.out.println("task " + myID + " done in thread" + Thread.currentThread().getName());  
    }
    
	   	
	// Κώδικας που εκτελεί το κάθε νήμα - εργαζόμενος 
	private static class Worker extends Thread
	{
	    Runnable[] myTasks;
	    
	    public Worker (Runnable[] tasks)
	    {
	        myTasks = tasks;
	    }    
	    
		public void run()
		{
			int element;
			// Λαμβάνει εργασία (ή στοιχείο πίνακα) αν υπάρχει
			while ((element = getTask()) >= 0)
			{
				myTasks[element].run();
			}
		}
	}
}
