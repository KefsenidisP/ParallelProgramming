class VecSumSeq
{
	public static void main(String[] args)
	{  
		
		int size = 100000;
		double[] a = new double[size];

		for(int i = 0; i < size; i++)
			a[i] = i; 
			//a[i] = Math.random()*size; 
		
         /* for debugging 
		for(int i = 0; i < size; i++) 
			System.out.println(a[i]); */

        double sum = 0.0;

		// get current time 
		long start = System.currentTimeMillis();

		for(int i = 0; i < size; i++) 
			sum = sum + a[i];
               
		// get current time and calculate elapsed time
		long elapsedTimeMillis = System.currentTimeMillis()-start;
		System.out.println("time in ms = "+ elapsedTimeMillis);

		/* for debugging */
		System.out.println(sum); 
	}

}
