#include "test_functions.h"

#define ORT_ABORT_ON_ERROR(expr, g_ort)                      \
  do {                                                       \
    OrtStatus* onnx_status = (expr);                         \
    if (onnx_status != NULL) {                               \
      const char* msg = g_ort->GetErrorMessage(onnx_status); \
      fprintf(stderr, "Error: %s\n", msg);                   \
      g_ort->ReleaseStatus(onnx_status);                     \
      abort();                                               \
    }                                                        \
  } while (0)

void test_resize_image() {
    const char* bmp_filename = "tests/samples/fingerprint_image.bmp";
    unsigned char* img = NULL;
    int width, height;
      
    // Read the BMP file
    if (read_bmp_image(bmp_filename, &img, &width, &height) != 0) {
        fprintf(stderr, "Failed to read BMP image.\n");
        return;
    }

    printf("Original Image Size: %dx%d\n", width, height);

    // Define the desired output dimensions (e.g., 224x224)
    int output_width = 224;
    int output_height = 224;
    unsigned char output_img[224 * 224];

    if (output_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for resized image.\n");
        free(img);
        return;
    }
    printf("Working\n");

    // Resize the image
    resize_image(img, output_img, width, height, output_width, output_height);

    // Verify the output size
    int input_size = width * height;
    int output_size = sizeof(output_img);

    printf("Input Image Size: %d pixels\n", input_size);
    printf("Output Image Size: %d pixels\n", output_size);
    
    
    // Print some pixel values for visual confirmation
    printf("Resized Image (Partial Data):\n");
    for (int y = 0; y < 10; y++) {  // Print first 10 rows
        for (int x = 0; x < 10; x++) {  // Print first 10 columns
            printf("%3d ", output_img[y * output_width + x]);
        }
        printf("\n");
    }

    printf("Test passed! Resizing.\n");
    

    // Clean up
    free(img);

}

void test_normalize_image() {
    const char* bmp_filename = "tests/samples/fingerprint_image.bmp";
    unsigned char* img = NULL;
    int width, height;
      
    // Read the BMP file
    if (read_bmp_image(bmp_filename, &img, &width, &height) != 0) {
        fprintf(stderr, "Failed to read BMP image.\n");
        return;
    }

    // Define the desired output dimensions (e.g., 224x224)
    int output_width = 224;
    int output_height = 224;
    
    // Allocate memory for resized image
    unsigned char* resized_img = (unsigned char*)malloc(output_width * output_height * sizeof(unsigned char));
    if (resized_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for resized image.\n");
        free(img);
        return;
    }

    // Allocate memory for normalized image
    float* normalized_img = (float*)malloc(output_width * output_height * sizeof(float));
    if (normalized_img == NULL) {
        fprintf(stderr, "Failed to allocate memory for normalized image.\n");
        free(img);
        free(resized_img);
        return;
    }

    // Resize the image
    resize_image(img, resized_img, width, height, output_width, output_height);

    // Normalize the image
    normalize_image(resized_img, normalized_img, output_width, output_height);

    // Verify all elements of normalized_img are in [0, 1]
    bool valid = true;
    for (int i = 0; i < output_width * output_height; i++) {
        if (normalized_img[i] < 0.0f || normalized_img[i] > 1.0f) {
            valid = false;
            printf("Error: Value out of bounds at index %d: %f\n", i, normalized_img[i]);
            break;
        }
    }

    if (valid) {
        printf("All elements of normalized_img are in the [0, 1] interval.\n");
    }
    else {
        printf("Test failed: Some elements are outside the [0, 1] interval.\n");
    }

    // Clean up
    free(img);
    free(resized_img);
    free(normalized_img);
}


void test_load_model(const OrtApi* g_ort, const ORTCHAR_T* model_path) {
    if (g_ort == NULL || model_path == NULL) {
        fprintf(stderr, "Test failed: Invalid parameters.\n");
        return;
    }

    OrtEnv* env = NULL;
    OrtSession* session = NULL;

    // load_model 호출
    int result = load_model(g_ort, model_path, &env, &session);
    if (result != 0) {
        fprintf(stderr, "Test failed: Unable to load model '%s'.\n", model_path);
        return;
    }

    // 입력 및 출력 노드 수 확인
    size_t num_input_nodes, num_output_nodes;
    ORT_ABORT_ON_ERROR(g_ort->SessionGetInputCount(session, &num_input_nodes), g_ort);
    ORT_ABORT_ON_ERROR(g_ort->SessionGetOutputCount(session, &num_output_nodes), g_ort);

    printf("Model loaded successfully.\n");
    printf("Number of inputs: %zu\n", num_input_nodes);
    printf("Number of outputs: %zu\n", num_output_nodes);

    // 리소스 해제
    g_ort->ReleaseSession(session);
    g_ort->ReleaseEnv(env);
}

void test_run_model(const OrtApi* g_ort, OrtSession* session) {
    if (g_ort == NULL || session == NULL) {
        fprintf(stderr, "Test failed: Invalid parameters.\n");
        return;
    }

    // 테스트 입력 데이터 1
    float input_data1[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
    size_t input_size1 = 4;

    // 테스트 입력 데이터 2
    float input_data2[4] = { 5.0f, 6.0f, 7.0f, 8.0f };
    size_t input_size2 = 4;

    // 출력 데이터 버퍼
    float output_data[4] = { 0 };
    size_t output_size = 4;

    // 모델 실행 테스트
    int result = run_model(g_ort, session, input_data1, input_size1, input_data2, input_size2, output_data, output_size);
    if (result != 0) {
        fprintf(stderr, "Test failed: Unable to run model.\n");
        return;
    }

    // 출력 결과 확인
    printf("Model output:\n");
    for (size_t i = 0; i < output_size; i++) {
        printf("Output[%zu] = %f\n", i, output_data[i]);
    }

    printf("Test passed: Model ran successfully.\n");
}