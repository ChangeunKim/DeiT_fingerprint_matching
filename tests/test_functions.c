

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


void test_load_model(const ORTCHAR_T* model_path) {
    const OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (g_ort == NULL) {
        fprintf(stderr, "Test failed: Failed to initialize ONNX Runtime API.\n");
        return -1;
    }

    if (model_path == NULL) {
        fprintf(stderr, "Test failed: Invalid file path.\n");
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

void test_run_model(const ORTCHAR_T* model_path, const char* image1, const char* image2) {
    const OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (g_ort == NULL) {
        fprintf(stderr, "Test failed: Failed to initialize ONNX Runtime API.\n");
        return;
    }

    if (model_path == NULL || image1 == NULL || image2 == NULL) {
        fprintf(stderr, "Test failed: Invalid file paths.\n");
        return;
    }

    OrtEnv* env = NULL;
    OrtSession* session = NULL;

    // 모델 및 세션 로드
    if (load_model(g_ort, model_path, &env, &session) != 0) {
        fprintf(stderr, "Test failed: Failed to load model.\n");
        return;
    }

    // Load input data (using two BMP images as input)
    unsigned char* raw_data1 = NULL;
    unsigned char* raw_data2 = NULL;
    float* input_data1 = (float*)malloc(3 * 224 * 224 * sizeof(float));
    float* input_data2 = (float*)malloc(3 * 224 * 224 * sizeof(float));
    size_t input_height, input_width;

    if (read_bmp_image(image1, &raw_data1, &input_width, &input_height) != 0) {
        fprintf(stderr, "Failed to load fingerprint image 1.\n");
        g_ort->ReleaseSession(session);
        g_ort->ReleaseEnv(env);
        return;
    }

    if (read_bmp_image(image2, &raw_data2, &input_width, &input_height) != 0) {
        fprintf(stderr, "Failed to load fingerprint image 2.\n");
        free(raw_data1);
        free(input_data1);
        g_ort->ReleaseSession(session);
        g_ort->ReleaseEnv(env);
        return;
    }

    // Preprocess image 1
    preprocess_image(raw_data1, (unsigned char*)input_data1, input_width, input_height, 224, 224);

    // Preprocess image 2
    preprocess_image(raw_data2, (unsigned char*)input_data2, input_width, input_height, 224, 224);

    // Clean up raw data
    free(raw_data1);
    free(raw_data2);


   
    // 출력 데이터 버퍼: 각 출력의 shape은 [1, 64] 이므로 64 크기의 버퍼 필요
    float output_data1[64] = { 0 };
    float output_data2[64] = { 0 };
    
    int result = run_model(g_ort, session, input_data1, 3 * 224 * 224, input_data2, 3 * 224 * 224,
        output_data1, 64, output_data2, 64);
    if (result != 0) {
        fprintf(stderr, "Test failed: Unable to run model.\n");
        free(input_data1);
        free(input_data2);
        g_ort->ReleaseSession(session);
        g_ort->ReleaseEnv(env);
        return;
    }

    // 출력 결과 확인
    printf("Model output 1:\n");
    for (size_t i = 20; i < 30; i++) {
        printf("Output1[%zu] = %f\n", i, output_data1[i]);
    }

    printf("Model output 2:\n");
    for (size_t i = 20; i < 30; i++) {
        printf("Output2[%zu] = %f\n", i, output_data2[i]);
    }

    // 메모리 해제
    free(input_data1);
    free(input_data2);
    g_ort->ReleaseSession(session);
    g_ort->ReleaseEnv(env);
}
