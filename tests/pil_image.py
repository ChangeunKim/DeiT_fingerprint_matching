from PIL import Image
import numpy as np

# Load the BMP file with PIL
bmp_image = Image.open("tests/samples/fingerprint_image.bmp")
print("Mode:", bmp_image.mode)  # RGB, RGBA, L (grayscale), etc.
print("Size:", bmp_image.size)  # (width, height)
print("First pixel:", bmp_image.getdata()[0])  # First pixel value

# Save the BMP pixel data as a binary file for comparison
bmp_array = np.array(bmp_image)
bmp_array.flatten().tofile("tests/samples/bmp_reference.bin")