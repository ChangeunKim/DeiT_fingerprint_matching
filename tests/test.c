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
void check_file_access(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file) {
        printf("File '%s' exists and is accessible.\n", filepath);
        fclose(file);
    }
    else {
        perror("Error accessing file");
        printf("File '%s' does not exist or cannot be accessed.\n", filepath);
    }
}



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
    const char* model_path = "C:/Users/owner/source/repos/DeiT_fingerprint_matching/tests/optimized_deit_tiny_siamese.onnx";
    check_file_access(model_path);

    #ifdef _WIN32
    wchar_t* w_model_path = convert_to_wchar(model_path);
    if (!w_model_path) {
        fprintf(stderr, "Failed to convert model path to wide char.\n");
        return -1;
    }
    #else
    const char* w_model_path = model_path;  // Unix에서는 UTF-8 사용 가능
    #endif

    
    const OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (g_ort == NULL) {
        fprintf(stderr, "Failed to initialize ONNX Runtime API.\n");
        return -1;
    }

    test_load_model(g_ort, w_model_path);
    printf("Completed test: Load Model\n\n");
    

    printf("Running test: Run Model\n");

    OrtEnv* env = NULL;
    OrtSession* session = NULL;

    // 모델 로드 및 세션 생성
    if (load_model(g_ort, w_model_path, &env, &session) != 0) {
        fprintf(stderr, "Failed to load model.\n");
        return -1;
    }

    // 모델 실행 테스트
    test_run_model(g_ort, session);

    // 리소스 해제
    g_ort->ReleaseSession(session);
    g_ort->ReleaseEnv(env);
    printf("Completed test: Run Model\n\n");


#ifdef _WIN32
    free(w_model_path);
#endif

    return 0;
}