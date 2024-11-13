#include "template.h"

int read_bmp_image(const char* filename, unsigned char** img, int* width, int* height, uint16_t* bpp) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening BMP file");
        return -1;
    }

    // Read BMP file header
    uint16_t bfType;
    fread(&bfType, sizeof(uint16_t), 1, file);
    if (bfType != 0x4D42) {  // 'BM' in hexadecimal
        fclose(file);
        fprintf(stderr, "Not a valid BMP file.\n");
        return -1;
    }

    fseek(file, 18, SEEK_SET);  // Move to the width/height section
    fread(width, sizeof(int), 1, file);   // Image width
    fread(height, sizeof(int), 1, file);  // Image height

    // Save the original height sign
    int original_height = *height;

    // Take the absolute value of height for memory allocation and other computations
    *height = abs(*height);

    // Read bits per pixel (bpp) from the BMP header (at offset 28)
    fseek(file, 28, SEEK_SET);  // Move to the bits per pixel section
    fread(bpp, sizeof(uint16_t), 1, file);

    int row_padded = (*width * 3 + 3) & (~3); // Each row is padded to a multiple of 4 bytes
    *img = (unsigned char*)malloc(row_padded * (*height));
    if (*img == NULL) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed.\n");
        return -1;
    }

    fseek(file, 54, SEEK_SET); // Move to the pixel data
    fread(*img, 1, row_padded * (*height), file);

    // Reverse the rows if the original height was negative (top-down DIB)
    if (original_height < 0) {
        int half = *height / 2;
        for (int i = 0; i < half; i++) {
            // Swap rows i and height - i - 1
            for (int j = 0; j < row_padded; j++) {
                unsigned char temp = (*img)[i * row_padded + j];
                (*img)[i * row_padded + j] = (*img)[(*height - i - 1) * row_padded + j];
                (*img)[(*height - i - 1) * row_padded + j] = temp;
            }
        }
    }

    fclose(file);
    return 0;
}

void resize_image(unsigned char* input_img, unsigned char* output_img, int width, int height) {
    
}

void normalize_image(unsigned char* input_img, unsigned char* output_img) {
    
}

void preprocess_image(unsigned char* input_img, unsigned char* output_img, int width, int height) {
    unsigned char* resized_img = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    resize_image(input_img, resized_img, width, height);
    normalize_image(resized_img, output_img);
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
    uint16_t bpp;

    // Read the image file
    if (read_bmp_image(image_filename, &img, &width, &height, &bpp) != 0) {
        return -1;  // Error reading image
    }

    // Preprocess image
    unsigned char output_img[224 * 224];  // Preprocessed image (224x224)
    preprocess_image(img, output_img, 224, 224);
    free(img);

    // load_model();
    // run_model();

    return 0;
}