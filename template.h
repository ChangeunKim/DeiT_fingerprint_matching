#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <onnxruntime_c_api.h>
#include "config.h"

// Image
int read_bmp_image(const char* filename, unsigned char** img, int* width, int* height);
void apply_box_filter(unsigned char* input_img, unsigned char* output_img,
    int width, int height, int box_size);
unsigned char interpolate_linear(unsigned char* image, int width, int height, int channel, float x, float y);
void gaussian_blur(unsigned char* input_img, unsigned char* output_img,
    int width, int height, int kernel_size);
void resize_image(unsigned char* input_img, unsigned char* output_img, int input_width, int input_height, int output_width, int output_height);
void normalize_image(unsigned char* input_img, float* output_img, int output_width, int output_height);
void preprocess_image(unsigned char* input_img, float* output_img, int input_width, int input_height, int output_width, int output_height);
void reshape_image(float* original_image, float* reshaped_image, int width, int height, int channels);

// ONNX Model
int run_model(const OrtApi* g_ort, OrtSession* session, float* input_data1, size_t input_size1,
    float* input_data2, size_t input_size2, float* output_data1, size_t output_size1,
    float* output_data2, size_t output_size2);

// API function
DllAPI int load_model(const OrtApi* g_ort, const ORTCHAR_T* model_path, OrtEnv** out_env, OrtSession** out_session);
DllAPI int generate_template(const char* image_filename, const OrtApi* g_ort, OrtEnv* env, OrtSession* session, float* output_template);
DllAPI void clean_model(const OrtApi* g_ort, OrtEnv* env, OrtSession* session);

#endif // TEMPLATE_H
