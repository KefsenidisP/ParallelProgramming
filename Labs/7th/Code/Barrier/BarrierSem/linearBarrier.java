import java.util.concurrent.Semaphore;

class linearBarrier {
	private int arrived = 0;
	private int totalThreads;
	private Semaphore waiting = new Semaphore(1);
	private Semaphore leaving = new Semaphore(0);
	
	public linearBarrier (int total) {
		this.totalThreads = total;
	}		

	public void barrier()
	{
		try {
			waiting.acquire();
			arrived++;
			System.out.println("waiting "+Thread.currentThread().getName());
			if (arrived == totalThreads) leaving.release();
		} catch (InterruptedException e) {};
		waiting.release();
		
		try {
			System.out.println("leaving "+Thread.currentThread().getName());
			leaving.acquire();
		} catch (InterruptedException e) {};
		leaving.release();
	}
}
