#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <onnxruntime_c_api.h>

// BMP file header
typedef struct {
    unsigned short file_type;      // File type (should be 'BM')
    unsigned int file_size;        // Size of the BMP file
    unsigned short reserved1;      // Reserved; must be zero
    unsigned short reserved2;      // Reserved; must be zero
    unsigned int offset_data;      // Offset to start of pixel data
} BMPFileHeader;

// BMP info header
typedef struct {
    unsigned int size;             // Size of this header
    int width;                     // Width of the bitmap in pixels
    int height;                    // Height of the bitmap in pixels
    unsigned short planes;         // Number of color planes, must be 1
    unsigned short bit_count;      // Number of bits per pixel (24 for RGB)
    unsigned int compression;      // Compression type (0 for uncompressed)
    unsigned int size_image;       // Image size (can be 0 for uncompressed)
    int x_pixels_per_meter;        // Horizontal resolution (pixels per meter)
    int y_pixels_per_meter;        // Vertical resolution (pixels per meter)
    unsigned int colors_used;      // Number of colors in the color palette
    unsigned int colors_important; // Important colors (0 = all colors are important)
} BMPInfoHeader;

// Image
int read_bmp_image(const char* filename, unsigned char** img, int* width, int* height, uint16_t* bpp);
void resize_image(unsigned char* input_img, unsigned char* output_img, int width, int height);
void normalize_image(unsigned char* input_img, unsigned char* output_img);
void preprocess_image(unsigned char* input_img, unsigned char* output_img, int width, int height);

// ONNX Model
int load_model(const char* filename, OrtSession** out_session);
int run_model(OrtSession* session, float* input_template);

// API function
int generate_template(const char* image_filename, const char* model_filename, float* input_template);

#endif // TEMPLATE_H