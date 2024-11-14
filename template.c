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

// Function to resize an image using bilinear interpolation
void resize_image(unsigned char* input_img, unsigned char* output_img,
    int input_width, int input_height,
    int output_width, int output_height) {

    float x_ratio = (float)(input_width - 1) / (output_width - 1);
    float y_ratio = (float)(input_height - 1) / (output_height - 1);

    for (int y = 0; y < output_height; y++) {
        for (int x = 0; x < output_width; x++) {
            float gx = x * x_ratio;
            float gy = y * y_ratio;

            int x0 = (int)gx;
            int y0 = (int)gy;
            int x1 = (x0 + 1 < input_width) ? x0 + 1 : x0;
            int y1 = (y0 + 1 < input_height) ? y0 + 1 : y0;

            float wx = gx - x0;
            float wy = gy - y0;

            // Pixel values at the four corners
            unsigned char TL = input_img[y0 * input_width + x0];
            unsigned char TR = input_img[y0 * input_width + x1];
            unsigned char BL = input_img[y1 * input_width + x0];
            unsigned char BR = input_img[y1 * input_width + x1];

            // Bilinear interpolation
            float top = (1 - wx) * TL + wx * TR;
            float bottom = (1 - wx) * BL + wx * BR;
            float value = (1 - wy) * top + wy * bottom;

            output_img[y * output_width + x] = (unsigned char)(value + 0.5f);  // Round to nearest
        }
    }
}

void normalize_image(unsigned char* input_img, float* output_img, int output_width, int output_height) {
    for (int i = 0; i < output_width * output_height; i++) {
        output_img[i] = input_img[i] / 255.0f;  // Normalize to [0, 1]
    }
}


void preprocess_image(unsigned char* input_img, unsigned char* output_img, int input_width, int input_height, int output_width, int output_height) {
    unsigned char* resized_img = (unsigned char*)malloc(output_width * output_height * sizeof(unsigned char));
    resize_image(input_img, resized_img,input_width, input_height, output_width, output_height);
    normalize_image(resized_img, output_img, output_width, output_height);
    free(resized_img);
}

int load_model(const char* filename, OrtSession** out_session) {

    // 1. load onnx file
    // 2. create onnx runtime session

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
