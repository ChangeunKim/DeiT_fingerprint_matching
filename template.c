#include "template.h"

int read_bmp_image(const char* filename, unsigned char** img) {
    
    // Do not use opencv if possible

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

    // Read the image file
    if (read_bmp_image(image_filename, &img) != 0) {
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