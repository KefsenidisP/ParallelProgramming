import java.io.File;
import java.io.IOException;
import java.awt.Color;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;

public class RGBtoGrayScale {
   public static void main(String args[]) {
		
		String fileNameR = null;
		String fileNameW = null;
		
		//Input and Output files using command line arguments
		if (args.length != 2) {
			System.out.println("Usage: java RGBtoGrayScale <file to read > <file to write>");
			System.exit(1);
		}
		fileNameR = args[0];
		fileNameW = args[1];
		
		//The same without command line arguments
		// fileNameR = "original.jpg";
		// fileNameW = "new.jpg";
				
		//Reading Input file to an image
		BufferedImage img = null;
		try {
		    img = ImageIO.read(new File(fileNameR));
		} catch (IOException e) {}

        //Start timing
		long start = System.currentTimeMillis(); 
		
		//Coefficinets of R G B to GrayScale
		double redCoefficient = 0.299;
		double greenCoefficient = 0.587;
		double blueCoefficient = 0.114;
      
		for (int y = 0; y < img.getHeight(); y++) {
			for (int x = 0; x < img.getWidth(); x++) {
				//Retrieving contents of a pixel
				int pixel = img.getRGB(x,y);
				//Creating a Color object from pixel value
				Color color = new Color(pixel, true);
				//Retrieving the R G B values, 8 bits per r,g,b
				//Calculating GrayScale
				int red = (int) (color.getRed() * redCoefficient);
				int green = (int) (color.getGreen() * greenCoefficient);
				int blue = (int) (color.getBlue() * blueCoefficient);
				//Creating new Color object
				color = new Color(red+green+blue, 
				                  red+green+blue, 
				                  red+green+blue);
				//Setting new Color object to the image
				img.setRGB(x, y, color.getRGB());
			}
		}
	
	    //Stop timing
	    long elapsedTimeMillis = System.currentTimeMillis()-start;
       
		//Saving the modified image to Output file
		try {
		  File file = new File(fileNameW);
		  ImageIO.write(img, "jpg", file);
		} catch (IOException e) {}
      
		System.out.println("Done...");
		System.out.println("time in ms = "+ elapsedTimeMillis);
   }
}
