#include "../template.h"

// Compare the image data with the reference data byte by byte and log mismatches
void compare_images_unit(unsigned char* img, unsigned char* raw_img, unsigned char* reference_data, long img_data_size) {
    int mismatch_count = 0;  // Count mismatched pixels
    unsigned char tolerance = 1; // Allow a tolerance for byte-by-byte comparison

    long num_pixels = img_data_size / 3;  // Assuming 3 channels (RGB)

    for (long i = 0; i < num_pixels; i++) {
        for (int c = 0; c < 3; c++) {  // Iterate over color channels (RGB)
            long index = i * 3 + c;  // Calculate index for the current channel

            if (abs(img[index] - reference_data[index]) > tolerance) {
                mismatch_count++;
                fprintf(stderr, "Error: Image data does not match reference at pixel %ld, channel %d: img[%ld] = %u, reference[%ld] = %u\n",
                    i, c, index, img[index], index, reference_data[index]);
            }
        }
    }

    if (mismatch_count == 0) {
        printf("All pixels match the reference image within tolerance.\n");
    }
    else {
        fprintf(stderr, "Total mismatches (within tolerance): %d out of %ld pixels\n", mismatch_count, img_data_size);
        free(img);
        if (raw_img != NULL)
            free(raw_img);
        free(reference_data);
        exit(1);
    }
}

void compare_images_float(float* img, unsigned char* raw_img, float* reference_data, long img_data_size) {
    int mismatch_count = 0;  // Count mismatched pixels
    float tolerance = 0.05; // Allow a tolerance for byte-by-byte comparison

    long num_pixels = img_data_size / (sizeof(float) * 3);;  // Assuming 3 channels (RGB)

    for (long i = 0; i < num_pixels; i++) {
        for (int c = 0; c < 3; c++) {  // Iterate over color channels (RGB)
            long index = i * 3 + c;  // Calculate index for the current channel

            if (fabs(img[index] - reference_data[index]) > tolerance) {
                mismatch_count++;
                fprintf(stderr, "Error: Image data does not match reference at pixel %ld, channel %d: img[%ld] = %.6f, reference[%ld] = %.6f\n",
                    i, c, index, img[index], index, reference_data[index]);
            }
        }
    }

    if (mismatch_count == 0) {
        printf("All pixels match the reference image within tolerance.\n");
    }
    else {
        fprintf(stderr, "Total mismatches (within tolerance): %d out of %ld pixels\n", mismatch_count, img_data_size);
        free(img);
        if (raw_img != NULL)
            free(raw_img);
        free(reference_data);
        exit(1);
    }
}

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
        fprintf(stderr, "Error: Image data size (%ld) does not match reference size (%ld)\n", img_data_size, reference_size);
        free(img_data);
        free(reference_data);
        exit(1);
    }
    else {
        printf("Image size matches the reference size: %ld bytes\n", img_data_size);
    }

    // Compare the image data with the reference data byte by byte
    compare_images_unit(img_data, NULL, reference_data, img_data_size);

    printf("Test passed: BMP data matches the reference.\n");

    // Free allocated memory
    free(img_data);
    free(reference_data);
}

void test_resize_image() {
    const char* bmp_filename = "tests/samples/fingerprint_image.bmp";
    const char* reference_filename = "tests/samples/resized_image_reference.bin";
    unsigned char* img = NULL;
    int width, height;

    // Read the BMP file
    if (read_bmp_image(bmp_filename, &img, &width, &height) != 0) {
        fprintf(stderr, "Failed to read BMP image.\n");
        return;
    }

    printf("Original Image Size: %dx%d\n", width, height);

    // Define the desired output dimensions
    int output_width = 224;
    int output_height = 224;

    unsigned char* resized_img = (unsigned char*)malloc(output_width * output_height * 3);

    if (resized_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for resized image.\n");
        free(img);
        return;
    }
    printf("Working\n");

    // Resize the image
    resize_image(img, resized_img, width, height, output_width, output_height);

    // Open the reference binary file
    FILE* reference_file = fopen(reference_filename, "rb");
    if (reference_file == NULL) {
        fprintf(stderr, "Error: Failed to open reference file\n");
        free(img);
        free(resized_img);
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
        free(img);
        free(resized_img);
        exit(1);
    }
    fread(reference_data, 1, reference_size, reference_file);
    fclose(reference_file);
    
    // Verify that the size matches the expected BMP data size
    long img_data_size = 224 * 224 * 3;  // Considering bpp (bits per pixel)
    
    if (img_data_size != reference_size) {
        fprintf(stderr, "Error: Image data size (%ld) does not match reference size (%ld)\n", img_data_size, reference_size);
        free(img);
        free(resized_img);
        free(reference_data);
        exit(1);
    }
    else {
        printf("Image size matches the reference size: %ld bytes\n", img_data_size);
    }

    // Compare the image data with the reference data byte by byte
    compare_images_unit(resized_img, img, reference_data, img_data_size);

    printf("Test passed. Resizing matches the reference.\n");

    // Clean up
    free(img);
    free(resized_img);
}

void test_normalize_image() {
    const char* bmp_filename = "tests/samples/fingerprint_image.bmp";
    const char* reference_filename = "tests/samples/normalized_image_reference.bin";
    unsigned char* img = NULL;
    int width, height;

    // Read the BMP file
    if (read_bmp_image(bmp_filename, &img, &width, &height) != 0) {
        fprintf(stderr, "Failed to read BMP image.\n");
        return;
    }

    // Allocate memory for normalized image
    float* normalized_img = (float*)malloc(width * height * 3 * sizeof(float));
    if (normalized_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for normalized image.\n");
        free(img);
        return;
    }

    // Normalize the image
    normalize_image(img, normalized_img, width, height);

    // Open the reference binary file
    FILE* reference_file = fopen(reference_filename, "rb");
    if (reference_file == NULL) {
        fprintf(stderr, "Error: Failed to open reference file\n");
        free(img);
        free(normalized_img);
        exit(1);
    }

    // Get size of the reference file
    fseek(reference_file, 0, SEEK_END);
    long reference_size = ftell(reference_file);
    fseek(reference_file, 0, SEEK_SET);

    // Allocate buffer for reference data and read it
    float* reference_data = (float*)malloc(reference_size);
    if (reference_data == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(reference_file);
        free(img);
        free(normalized_img);
        exit(1);
    }
    fread(reference_data, 1, reference_size, reference_file);
    fclose(reference_file);

    // Verify that the size matches the expected BMP data size
    long img_data_size = width * height * 3 * sizeof(float);  // Considering bpp (bits per pixel)
    
    if (img_data_size != reference_size) {
        fprintf(stderr, "Error: Image data size (%ld) does not match reference size (%ld)\n", img_data_size, reference_size);
        free(img);
        free(normalized_img);
        free(reference_data);
        exit(1);
    }
    else {
        printf("Image size matches the reference size: %ld bytes\n", img_data_size);
    }

    // Compare the image data with the reference data byte by byte
    compare_images_float(normalized_img, img, reference_data, img_data_size);

    // Clean up
    free(img);
    free(normalized_img);
    free(reference_data);
}

void test_preprocess_image() {
    const char* bmp_filename = "tests/samples/fingerprint_image.bmp";
    const char* reference_filename = "tests/samples/resized_and_normalized_image_reference.bin";
    unsigned char* img = NULL;
    int width, height;

    // Read the BMP file
    if (read_bmp_image(bmp_filename, &img, &width, &height) != 0) {
        fprintf(stderr, "Failed to read BMP image.\n");
        return;
    }

    printf("Original Image Size: %dx%d\n", width, height);

    // Define the desired output dimensions
    int output_width = 224;
    int output_height = 224;

    // Allocate memory for normalized image
    float* preprocessed_img = (float*)malloc(output_width * output_height * 3 * sizeof(float));
    if (preprocessed_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for normalized image.\n");
        free(img);
        return;
    }

    // Normalize the image
    preprocess_image(img, preprocessed_img, width, height, output_width, output_height);

    // Open the reference binary file
    FILE* reference_file = fopen(reference_filename, "rb");
    if (reference_file == NULL) {
        fprintf(stderr, "Error: Failed to open reference file\n");
        free(img);
        exit(1);
    }

    // Get size of the reference file
    fseek(reference_file, 0, SEEK_END);
    long reference_size = ftell(reference_file);
    fseek(reference_file, 0, SEEK_SET);

    // Allocate buffer for reference data and read it
    float* reference_data = (float*)malloc(reference_size);
    if (reference_data == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(reference_file);
        free(img);
        exit(1);
    }
    fread(reference_data, 1, reference_size, reference_file);
    fclose(reference_file);

    // Verify that the size matches the expected BMP data size
    long img_data_size = 224 * 224 * 3 * sizeof(float);  // Considering bpp (bits per pixel)

    if (img_data_size != reference_size) {
        fprintf(stderr, "Error: Image data size (%ld) does not match reference size (%ld)\n", img_data_size, reference_size);
        free(img);
        free(preprocessed_img);
        free(reference_data);
        exit(1);
    }
    else {
        printf("Image size matches the reference size: %ld bytes\n", img_data_size);
    }

    // Compare the image data with the reference data byte by byte
    compare_images_float(preprocessed_img, img, reference_data, img_data_size);

    free(img);
    free(preprocessed_img);
    free(reference_data);
}

/*
void test_load_model() {
    const char* model_path = "C:/Users/owner/source/repos/DeiT_fingerprint_matching/tests/optimized_deit_tiny_siamese.onnx";
    OrtEnv* env = NULL;
    OrtSession* session = NULL;

    printf("Testing model loading: %s\n", model_path);

    // Attempt to load the model
    int result = load_model(model_path, &env, &session);
    if (result != 0 || session == NULL) {
        fprintf(stderr, "\nIn test_load_model() - Test failed: Unable to load model '%s'.\n\n", model_path);
        return;
    }

    printf("Test passed: Model '%s' loaded successfully.\n", model_path);

    // Verify the session by checking the input and output node counts
    const OrtApi* api = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    OrtStatus* status = NULL;

    // Get the number of input nodes
    size_t num_input_nodes;
    status = api->SessionGetInputCount(session, &num_input_nodes);
    if (status != NULL) {
        fprintf(stderr, "Error getting input count: %s\n", api->GetErrorMessage(status));
        api->ReleaseStatus(status);
    }
    else {
        printf("Number of inputs: %zu\n", num_input_nodes);
    }

    // Get the number of output nodes
    size_t num_output_nodes;
    status = api->SessionGetOutputCount(session, &num_output_nodes);
    if (status != NULL) {
        fprintf(stderr, "Error getting output count: %s\n", api->GetErrorMessage(status));
        api->ReleaseStatus(status);
    }
    else {
        printf("Number of outputs: %zu\n", num_output_nodes);
    }

    // Clean up
    api->ReleaseSession(session);
    api->ReleaseEnv(env);
    printf("Test completed successfully.\n");
}
*/
