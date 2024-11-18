from PIL import Image
import albumentations as A
from albumentations.pytorch import ToTensorV2
import numpy as np

# Define the transformations
resize_transform = A.Compose([
    A.Resize(224, 224)  # Resize only
])

normalize_transform = A.Compose([
    A.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])  # Normalize only
])

resize_and_normalize_transform = A.Compose([
    A.Resize(224, 224),  # Resize to 224x224
    A.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),  # ImageNet normalization
])

def preprocess_image(image_path, transform):
    # Load the image using PIL in RGB format
    image = Image.open(image_path).convert("RGB")
    
    # Convert the PIL image to a NumPy array
    image_np = np.array(image)

    # Apply the transformation
    transformed = transform(image=image_np)
    transformed_image = transformed['image']

    return transformed_image

# File paths
image_path = "tests/samples/fingerprint_image.bmp" 
resized_file_path = "tests/samples/resized_image_reference.bin"
normalized_file_path = "tests/samples/normalized_image_reference.bin"
resized_and_normalized_file_path = "tests/samples/resized_and_normalized_image_reference.bin"

# Resized image (224x224, unnormalized)
resized_image = preprocess_image(image_path, resize_transform)
resized_image.flatten().tofile(resized_file_path)

# Normalized image (original size, normalized)
normalized_image = preprocess_image(image_path, normalize_transform)
normalized_image.flatten().tofile(normalized_file_path)

# Resized and normalized image (224x224, normalized, as PyTorch tensor)
resized_and_normalized_image = preprocess_image(image_path, resize_and_normalize_transform)
resized_and_normalized_image.flatten().tofile(resized_and_normalized_file_path)

# Display shapes and verify each stage
print("Resized image shape:", resized_image.shape)  # Should be (224, 224, 3)
print("Normalized image shape:", normalized_image.shape)  # Should match original size
print("Resized and normalized image shape:", resized_and_normalized_image.shape)   # Should be (224, 224, 3)
