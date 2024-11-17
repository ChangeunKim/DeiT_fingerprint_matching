#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_functions.h"
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

void test_bmp_reader();
void test_cosine_similarity();




int main() {
    
    printf("Running test: BMP Reader\n");
    test_bmp_reader();
    printf("Completed test: BMP Reader\n\n");

    printf("Running test: Cosine Similarity\n");
    test_cosine_similarity();
    printf("Completed test: Cosine Similarity\n\n");

    printf("Running test: Resize Image\n");
    test_resize_image();
    printf("Completed test: Resize Image\n\n");


    printf("Running test: Normalize Image\n");
    test_normalize_image();
    printf("Completed test: Normalize Image\n\n");
    


    printf("Running test: Load Model\n");
    const wchar_t* model_path = L"C:/Users/owner/source/repos/DeiT_fingerprint_matching/tests/optimized_deit_tiny_siamese.onnx";  // path in unicode(utf-8)
    test_load_model(model_path);
    printf("Completed test: Load Model\n\n");
    

    printf("Running test: Run Model\n");
    const char* image1 = "tests/samples/fingerprint_image(1).bmp";
    const char* image2 = "tests/samples/fingerprint_image(10).bmp";
    test_run_model(model_path, image1, image2);
    printf("Completed test: Run Model\n\n");


    return 0;
}

