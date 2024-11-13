#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../template.h"

void test_bmp_reader() {
    const char* bmp_filename = "tests/samples/fingerprint_image.bmp";
    const char* reference_filename = "tests/samples/bmp_reference.bin";

    unsigned char* img_data = NULL;
    int width, height;

    // Call the BMP reader function to read the image
    if (read_bmp_image(bmp_filename, &img_data, &width, &height) != 0) {
        fprintf(stderr, "Error: Failed to read BMP file\n");
        exit(1);
    }

    // Open the reference binary file
    FILE* reference_file = fopen(reference_filename, "rb");
    if (reference_file == NULL) {
        fprintf(stderr, "Error: Failed to open reference file\n");
        free(img_data);
        exit(1);
    }

    // Get size of the reference file
    fseek(reference_file, 0, SEEK_END);
    long reference_size = ftell(reference_file);
    fseek(reference_file, 0, SEEK_SET);

    // Allocate buffer for reference data and read it
    unsigned char* reference_data = (unsigned char*)malloc(reference_size);
    if (reference_data == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(reference_file);
        free(img_data);
        exit(1);
    }
    fread(reference_data, 1, reference_size, reference_file);
    fclose(reference_file);

    // Verify that the size matches the expected BMP data size
    long img_data_size = width * height * 3;  // Considering bpp (bits per pixel)
    if (img_data_size != reference_size) {
        fprintf(stderr, "Error: Image data size does not match reference\n");
        free(img_data);
        free(reference_data);
        exit(1);
    }

    // Compare the image data with the reference data byte by byte
    for (long i = 0; i < img_data_size; i++) {
        if (img_data[i] != reference_data[i]) {
            fprintf(stderr, "Error: Image data does not match reference at byte %ld\n", i);
            free(img_data);
            free(reference_data);
            exit(1);
        }
    }

    printf("Test passed: BMP data matches the reference.\n");

    // Free allocated memory
    free(img_data);
    free(reference_data);
}


int main() {
    test_bmp_reader();
    return 0;
}