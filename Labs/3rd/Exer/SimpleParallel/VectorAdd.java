/* Vector Addition a = b + c                       */

class VectorAdd
{
  public static void main(String args[])
  {
    int size = 1000;
    
    double[] a = new double[size];
    double[] b = new double[size];
    double[] c = new double[size];
    for(int i = 0; i < size; i++) {
        a[i] = 0.0;
		b[i] = 1.0;
        c[i] = 0.5;
    }
		
    for (int i= 0; i < size; i++) {
        a[i] = b[i] + c[i];
    }

    /* for debugging 
    for(int i = 0; i < size; i++) 
        System.out.println(a[i]); */
  }
}
