/* Matrix Addition A = B + C                       */

class MatrixAdd
{
  public static void main(String args[])
  {
    int size = 10;
    
    double[][] a = new double[size][size];
    double[][] b = new double[size][size];
    double[][] c = new double[size][size];
    for(int i = 0; i < size; i++) 
      for(int j = 0; j < size; j++) { 
        a[i][j] = 0.1;
		b[i][j] = 0.3;
        c[i][j] = 0.5;
      }   
    
    for(int i = 0; i < size; i++) 
      for(int j = 0; j < size; j++)  
        a[i][j] = b[i][j] + c[i][j];

    // for debugging 
    for(int i = 0; i < size; i++) { 
      for(int j = 0; j < size; j++) 
        System.out.print(a[i][j]+" "); 
      System.out.println();
    }    
  }
}
