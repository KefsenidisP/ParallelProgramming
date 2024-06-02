import java.io.File;
import java.io.IOException;
import java.awt.Color;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;


public class ImageHistogram {
   public static void main(String args[]) throws IOException {
		//Reading the image
		String fileNameR = null;
		String fileNameW = null;
		
		if (args.length != 2) {
			System.out.println("Usage: java ImageHistogran <file to read ><file to write>");
			System.exit(1);
		}
		fileNameR = args[0];
		fileNameW = args[1];	
		
		BufferedImage img = null;
		try {
		    img = ImageIO.read(new File(fileNameR));
		} catch (IOException e) {}
		
		int[] r_hist = new int[256];
		int[] g_hist = new int[256];
		int[] b_hist = new int[256];
		int[] l_hist = new int[256];

		long start = System.currentTimeMillis(); 
		
		for (int y = 0; y < img.getHeight(); y++) {
		    int r,g,b,l;
			for (int x = 0; x < img.getWidth(); x++) {
				//Retrieving contents of a pixel
				int pixel = img.getRGB(x,y);
				//Creating a Color object from pixel value
				Color color = new Color(pixel, true);
				//Retrieving the R G B values, 8 bits per r,g,b
				//Calculating lumninance
				r = (int) color.getRed();
				g = (int) color.getGreen();
				b = (int) color.getBlue();
				l = (int) (r + g + b) / 3;
				//Updating histograms
				r_hist[r]++;
				g_hist[g]++;
				b_hist[b]++;
                l_hist[l]++;
			}
		}
	
	    long elapsedTimeMillis = System.currentTimeMillis()-start;
       
		//Saving histograms in file
		writeToFile(fileNameW, r_hist, g_hist, b_hist, l_hist);
			
		System.out.println("time in ms = "+ elapsedTimeMillis);
   }
   
   static void initHistograms(int[] r_hist, int[] g_hist, int[] b_hist, int[] l_hist) {
   
     for (int i=0; i<256; i++) {
         r_hist[i] = 0;
         g_hist[i] = 0;
         b_hist[i] = 0;
         l_hist[i] = 0;
     }
   }       
          
   static void writeToFile(String fileNameW, int[] r_hist, int[] g_hist, int[] b_hist, int[] l_hist) throws IOException{
    
    BufferedWriter writer = new BufferedWriter(new FileWriter(fileNameW, true));
    int stars;
    int scale = 5000;
    
    writer.append("Red Histogram\n");
    for (int i=0; i<256; i++) {
        stars = r_hist[i]/scale;
    	for (int j=0; j<stars; j++)
           writer.append("*");
        writer.append("\n");
    }
       
    writer.append("Blue Histogram\n");
    for (int i=0; i<256; i++) {
        stars = (int)(b_hist[i]/scale);
    	for (int j=0; j<stars; j++)
           writer.append("*");
        writer.append("\n");
    } 
         
    writer.append("Green Histogram\n");
    for (int i=0; i<256; i++) {
        stars = (int)(g_hist[i]/scale);
    	for (int j=0; j<stars; j++)
           writer.append("*");
        writer.append("\n");
    }
        
    writer.append("Luminance Histogram\n");
    for (int i=0; i<256; i++) {
        stars = (int)(l_hist[i]/scale);
    	for (int j=0; j<stars; j++)
           writer.append("*");
        writer.append("\n");  
    }      
    writer.close();
  }
}
