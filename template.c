#include "template.h"

int read_bmp_image(const char* filename, unsigned char** img, int* width, int* height) {
    // Open the BMP file
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening BMP file\n");
        return -1;
    }

    // Read the BMP header (first 18 bytes)
    unsigned char header[54];
    fread(header, 1, 54, file);

    // Check for BMP signature 'BM'
    if (header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "Error: Not a BMP file\n");
        fclose(file);
        return -1;
    }

    // Extract width, height, and bit depth (bits per pixel)
    *width = *(int*)&header[18];   // 4 bytes: width of the image
    *height = *(int*)&header[22];  // 4 bytes: height of the image
    short bpp = *(short*)&header[28];  // 2 bytes: bits per pixel (BPP)

    // Take the absolute value of height if it's negative (top-down or bottom-up)
    *height = abs(*height);

    // Check for valid bit depths (support 24-bit and 8-bit grayscale)
    if (bpp != 24 && bpp != 8) {
        fprintf(stderr, "Error: Unsupported BMP format (only 24-bit or 8-bit grayscale supported)\n");
        fclose(file);
        return -1;
    }

    // Calculate the total size of the image data
    int row_size = ((*width * bpp + 31) / 32) * 4; // Row size including padding
    int img_data_size = row_size * (*height);

    // Allocate memory for the image data
    *img = (unsigned char*)malloc(img_data_size);
    if (*img == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return -1;
    }

    // Read the pixel data
    fseek(file, *(int*)&header[10], SEEK_SET);  // Go to the pixel data offset
    fread(*img, 1, img_data_size, file);
    fclose(file);

    // If the image is grayscale (8-bit), convert it to RGB
    if (bpp == 8) {
        unsigned char* temp_img = (unsigned char*)malloc(*width * *height * 3);  // RGB buffer
        if (temp_img == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for grayscale to RGB conversion\n");
            free(*img);
            return -1;
        }

        // Convert grayscale to RGB
        for (int i = 0, j = 0; i < *height; i++) {
            for (int k = 0; k < *width; k++) {
                unsigned char gray = (*img)[i * row_size + k];
                temp_img[j++] = gray;  // Red
                temp_img[j++] = gray;  // Green
                temp_img[j++] = gray;  // Blue
            }
        }

        // Free the old image data and point to the new RGB data
        free(*img);
        *img = temp_img;
    }

    return 0;
}

// Helper function to interpolate pixel values (bilinear interpolation)
unsigned char interpolate(unsigned char* image, int w, int h, float x, float y) {
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1 < w ? x0 + 1 : x0;
    int y1 = y0 + 1 < h ? y0 + 1 : y0;

    float dx = x - x0;
    float dy = y - y0;

    // Interpolation for the R channel (we assume the image is RGB)
    float pixel = 0;
    pixel += (1 - dx) * (1 - dy) * image[(y0 * w + x0) * 3];  // R value at (x0, y0)
    pixel += dx * (1 - dy) * image[(y0 * w + x1) * 3];        // R value at (x1, y0)
    pixel += (1 - dx) * dy * image[(y1 * w + x0) * 3];         // R value at (x0, y1)
    pixel += dx * dy * image[(y1 * w + x1) * 3];               // R value at (x1, y1)

    // Return the pixel value clipped to [0, 255] range
    if (pixel < 0) pixel = 0;
    if (pixel > 255) pixel = 255;

    return (unsigned char)(pixel);
}

// Modified resize_image function using helper functions
void resize_image(unsigned char* input_img, unsigned char* output_img,
    int input_width, int input_height,
    int output_width, int output_height) {

    float x_ratio = (float)(input_width) / output_width;
    float y_ratio = (float)(input_height) / output_height;

    for (int y = 0; y < output_height; y++) {
        for (int x = 0; x < output_width; x++) {
            // Calculate the coordinates in the input image
            float gx = x * x_ratio;
            float gy = y * y_ratio;

            // Use bilinear interpolation for each color channel (R, G, B)
            for (int c = 0; c < 3; c++) {
                unsigned char interpolated_value = interpolate(input_img, input_width, input_height, gx, gy);

                // Set the pixel value for the output image
                output_img[(y * output_width + x) * 3 + c] = interpolated_value;
            }
        }
    }
}

void normalize_image(unsigned char* input_img, float* output_img, int output_width, int output_height) {
    // Mean and std for each channel (R, G, B)
    const float mean[3] = { 0.485f, 0.456f, 0.406f };
    const float std[3] = { 0.229f, 0.224f, 0.225f };

    int total_pixels = output_width * output_height;

    for (int i = 0; i < total_pixels; i++) {
        for (int c = 0; c < 3; c++) {  // Loop through channels
            int index = i * 3 + c;  // Calculate the index for the current channel
            // Normalize pixel value to [0, 1]
            float normalized_value = input_img[index] / 255.0f;
            // Apply mean and standard deviation normalization
            normalized_value = (normalized_value - mean[c]) / std[c];
            output_img[index] = normalized_value;
        }
    }
}


void preprocess_image(unsigned char* input_img, unsigned char* output_img, int input_width, int input_height, int output_width, int output_height) {
    unsigned char* resized_img = (unsigned char*)malloc(output_width * output_height * sizeof(unsigned char));
    resize_image(input_img, resized_img,input_width, input_height, output_width, output_height);
    normalize_image(resized_img, output_img, output_width, output_height);
    free(resized_img);
}

// Function to load an ONNX model and create an ONNX Runtime session
int load_model(const OrtApi* g_ort, const char* filename, OrtSession** out_session, OrtEnv** out_env) {
    if (g_ort == NULL || filename == NULL || out_session == NULL || out_env == NULL) {
        fprintf(stderr, "Invalid input parameters.\n");
        return -1;
    }

    OrtEnv* env = NULL;
    OrtSessionOptions* session_options = NULL;
    OrtStatus* status = NULL;

    // Initialize ONNX Runtime environment
    status = g_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntime", &env);
    if (status != NULL) {
        fprintf(stderr, "Failed to create ONNX Runtime environment: %s\n", g_ort->GetErrorMessage(status));
        g_ort->ReleaseStatus(status);
        return -1;
    }

    // Create session options
    status = g_ort->CreateSessionOptions(&session_options);
    if (status != NULL) {
        fprintf(stderr, "Failed to create session options: %s\n", g_ort->GetErrorMessage(status));
        g_ort->ReleaseEnv(env);
        g_ort->ReleaseStatus(status);
        return -1;
    }

    // Set optimization level to enable all optimizations
    g_ort->SetSessionGraphOptimizationLevel(session_options, ORT_ENABLE_ALL);

    // Load ONNX model and create session
    status = g_ort->CreateSession(env, filename, session_options, out_session);
    if (status != NULL) {
        fprintf(stderr, "Failed to load ONNX model '%s': %s\n", filename, g_ort->GetErrorMessage(status));
        g_ort->ReleaseSessionOptions(session_options);
        g_ort->ReleaseEnv(env);
        g_ort->ReleaseStatus(status);
        return -1;
    }

    printf("ONNX model '%s' loaded successfully.\n", filename);

    // Clean up session options
    g_ort->ReleaseSessionOptions(session_options);

    // Return the environment to be used outside
    *out_env = env;

    return 0;
}

int run_model(OrtSession* session, float* input_template) {

    // 1. create tensor
    // 2. run session

    return 0;
}

// API function
int generate_template(const char* image_filename, const char* model_filename, float* input_template) {

    unsigned char* img = NULL;
    int width, height;

    // Read the image file
    if (read_bmp_image(image_filename, &img, &width, &height) != 0) {
        return -1;  // Error reading image
    }

    // Preprocess image
    unsigned char output_img[224 * 224];  // Preprocessed image (224x224)
    preprocess_image(img, output_img, width, height, 224, 224);
    free(img);

    // load_model();
    // run_model();
    
    return 0;

}
