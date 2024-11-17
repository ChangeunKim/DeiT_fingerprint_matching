#ifndef TEST_RESIZE_IMAGE_H
#define TEST_RESIZE_IMAGE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <onnxruntime_c_api.h>
#include "../template.h"

// resize image
void test_resize_image();
void test_normalize_image();
void test_load_model(const ORTCHAR_T* model_path);
void test_run_model(const ORTCHAR_T* model_path, const char* image1, const char* image2);

#endif // TEST_RESIZE_IMAGE_H