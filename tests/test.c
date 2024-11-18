#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../template.h"



#ifdef _WIN32
#include <Windows.h>
#endif

// UTF-8 문자열을 wchar_t로 변환하는 함수 (Windows 전용)
#ifdef _WIN32
wchar_t* convert_to_wchar(const char* utf8_str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
    wchar_t* wstr = (wchar_t*)malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, wstr, len);
    return wstr;
}
#endif

void test_cosine_similarity();
void test_bmp_reader();
void test_resize_image();
void test_normalize_image();
void test_preprocess_image();
void test_load_model(const ORTCHAR_T* model_path);
void test_run_model(const ORTCHAR_T* model_path, const char* image1, const char* image2, float* output_data1, float* output_data2);
void test_verification(const float* embed1, const float* embed2);
void test_generate_template(const ORTCHAR_T* model_path, const char* image_filename);
void test_identification(const ORTCHAR_T* model_path);

int main() {

    printf("Running test: Cosine Similarity\n");
    test_cosine_similarity();
    printf("Completed test: Cosine Similarity\n\n");

    printf("Running test: BMP Reader\n");
    test_bmp_reader();
    printf("Completed test: BMP Reader\n\n");

    printf("Running test: Resize Image\n");
    test_resize_image();
    printf("Completed test: Resize Image\n\n");
    
    printf("Running test: Normalize Image\n");
    test_normalize_image();
    printf("Completed test: Normalize Image\n\n");

    printf("Running test: Preprocess Image\n");
    test_preprocess_image();
    printf("Completed test: Preprocess Image\n\n");

    printf("Running test: Load Model\n");
    const wchar_t* model_path = L"C:/Users/user/source/repos/DeiT_fingerprint_matching/models/optimized_deit_tiny_siamese.onnx";  // path in unicode(utf-8)
    test_load_model(model_path);
    printf("Completed test: Load Model\n\n");

    printf("Running test: Run Model\n");
    const char* image1 = "tests/samples/fingerprint_image(6).bmp";
    const char* image2 = "tests/samples/fingerprint_image(29).bmp";
    // Shape of output embedding: [1, 64]
    float* embed1 = (float*)malloc(64 * sizeof(float));
    float* embed2 = (float*)malloc(64 * sizeof(float));
    test_run_model(model_path, image1, image2, embed1, embed2);
    test_verification(embed1, embed2);
    printf("Completed test: Run Model\n\n");

    printf("Running test: Generate Template\n");
    test_generate_template(model_path, image1);
    printf("Completed test: Generate Template\n\n");

    printf("Running test: Fingerprint Identification\n");
    test_identification(model_path);
    printf("Completed test: Fingerprint Identification\n\n");

    return 0;
}

