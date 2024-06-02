
class MatMulTimeArgs
{
  public static void main(String args[])
  {
    int size = 0;

    if (args.length != 1) {
		System.out.println("Usage: java MatMulTimeArgs <size>");
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
		System.out.println("Attention: <size> should be >0");
		System.exit(1);
    }
 
    double[][] a = new double[size][size];
    double[][] b = new double[size][size];
    double[][] c = new double[size][size];
    for(int i = 0; i < size; i++) {
    	for(int j = 0; j < size; j++){
		    a[i][j] = 1;
		    b[i][j] = 2;
		    c[i][j] = 0;
        }
    }    
    
    long start = System.currentTimeMillis();
		
    for (int i= 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double sum = 0;
            for (int k = 0; k < size; k++) {
                sum = sum + a[i][k]*b[k][j];
            }
            c[i][j] = sum;
        }    
    }

    long elapsedTimeMillis = System.currentTimeMillis()-start;
    System.out.println("time in ms = "+ elapsedTimeMillis);

    /* for debugging */
    for(int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            System.out.println(c[i][j]); 
        } 
        System.out.println();
    } 
  }
}
