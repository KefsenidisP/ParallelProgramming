from PIL import Image

def rgbtogrey(img):
	# Get width and height of Image
    width, height = img.size

	# Coefficinets of r,g,b to GrayScale
    redCoefficient = 0.299
    greenCoefficient = 0.587
    blueCoefficient = 0.114

	# Iterate through each pixel
    for x in range(0, width):
        for y in range(0, height):
            # r,g,b value of pixel
            r, g, b = img.getpixel((x, y))

            r = int(r * redCoefficient)
            g = int(g * greenCoefficient) 
            b = int(b * blueCoefficient) 
            
            grey = r+b+g
            
            img.putpixel( (x, y), (grey, grey, grey, 255) ) 
    return img

# Read Image
orig = Image.open(r'original.jpg')

# Convert Image into RGB
img = orig.convert('RGB')

# call function
new = rgbtogrey(img)

# Write Image
new = new.save('new.jpg')
