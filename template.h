#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <math.h>
#include <stdlib.h>
#include <onnxruntime_c_api.h>

// Image
int read_bmp_image(const char* filename, unsigned char** img);
void resize_image(unsigned char* input_img, unsigned char* output_img, int width, int height);
void normalize_image(unsigned char* input_img, unsigned char* output_img);
void preprocess_image(unsigned char* input_img, unsigned char* output_img, int width, int height);

// ONNX Model
int load_model(const char* filename, OrtSession** out_session);
int run_model(OrtSession* session, float* input_template);

// API function
int generate_template(const char* image_filename, const char* model_filename, float* input_template);

#endif // TEMPLATE_H
