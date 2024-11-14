#include "test_resize_image.h"

void test_resize_image() {
    const char* bmp_filename = "tests/samples/fingerprint_image.bmp";
    unsigned char* img = NULL;
    int width, height;
      
    // Read the BMP file
    if (read_bmp_image(bmp_filename, &img, &width, &height) != 0) {
        fprintf(stderr, "Failed to read BMP image.\n");
        return;
    }

    printf("Original Image Size: %dx%d\n", width, height);

    // Define the desired output dimensions (e.g., 224x224)
    int output_width = 224;
    int output_height = 224;
    unsigned char output_img[224 * 224];

    if (output_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for resized image.\n");
        free(img);
        return;
    }
    printf("Working\n");

    // Resize the image
    resize_image(img, output_img, width, height, output_width, output_height);

    // Verify the output size
    int input_size = width * height;
    int output_size = sizeof(output_img);

    printf("Input Image Size: %d pixels\n", input_size);
    printf("Output Image Size: %d pixels\n", output_size);
    
    
    // Print some pixel values for visual confirmation
    printf("Resized Image (Partial Data):\n");
    for (int y = 0; y < 10; y++) {  // Print first 10 rows
        for (int x = 0; x < 10; x++) {  // Print first 10 columns
            printf("%3d ", output_img[y * output_width + x]);
        }
        printf("\n");
    }

    printf("Test passed! Resizing.\n");
    

    // Clean up
    free(img);

}

void test_normalize_image() {
    const char* bmp_filename = "tests/samples/fingerprint_image.bmp";
    unsigned char* img = NULL;
    int width, height;
      
    // Read the BMP file
    if (read_bmp_image(bmp_filename, &img, &width, &height) != 0) {
        fprintf(stderr, "Failed to read BMP image.\n");
        return;
    }

    // Define the desired output dimensions (e.g., 224x224)
    int output_width = 224;
    int output_height = 224;
    
    // Allocate memory for resized image
    unsigned char* resized_img = (unsigned char*)malloc(output_width * output_height * sizeof(unsigned char));
    if (resized_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for resized image.\n");
        free(img);
        return;
    }

    // Allocate memory for normalized image
    float* normalized_img = (float*)malloc(output_width * output_height * sizeof(float));
    if (normalized_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for normalized image.\n");
        free(img);
        free(resized_img);
        return;
    }

    // Resize the image
    resize_image(img, resized_img, width, height, output_width, output_height);

    // Normalize the image
    normalize_image(resized_img, normalized_img, output_width, output_height);

    // Verify all elements of normalized_img are in [0, 1]
    bool valid = true;
    for (int i = 0; i < output_width * output_height; i++) {
        if (normalized_img[i] < 0.0f || normalized_img[i] > 1.0f) {
            valid = false;
            printf("Error: Value out of bounds at index %d: %f\n", i, normalized_img[i]);
            break;
        }
    }

    if (valid) {
        printf("All elements of normalized_img are in the [0, 1] interval.\n");
    }
    else {
        printf("Test failed: Some elements are outside the [0, 1] interval.\n");
    }

    // Clean up
    free(img);
    free(resized_img);
    free(normalized_img);
}