#include "template.h"

#define ORT_ABORT_ON_ERROR(expr, g_ort)                      \
  do {                                                       \
    OrtStatus* onnx_status = (expr);                         \
    if (onnx_status != NULL) {                               \
      const char* msg = g_ort->GetErrorMessage(onnx_status); \
      fprintf(stderr, "Error: %s\n", msg);                   \
      g_ort->ReleaseStatus(onnx_status);                     \
      return -1;                                             \
    }                                                        \
  } while (0)


int read_bmp_image(const char* filename, unsigned char** img, int* width, int* height) {
    // Open the BMP file
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening BMP file\n");
        return -1;
    }

    // Read the BMP header (first 18 bytes)
    unsigned char header[54];
    fread(header, 1, 54, file);

    // Check for BMP signature 'BM'
    if (header[0] != 'B' || header[1] != 'M') {
        fprintf(stderr, "Error: Not a BMP file\n");
        fclose(file);
        return -1;
    }

    // Extract width, height, and bit depth (bits per pixel)
    *width = *(int*)&header[18];   // 4 bytes: width of the image
    *height = *(int*)&header[22];  // 4 bytes: height of the image
    short bpp = *(short*)&header[28];  // 2 bytes: bits per pixel (BPP)

    // Take the absolute value of height if it's negative (top-down or bottom-up)
    *height = abs(*height);

    // Check for valid bit depths (support 24-bit and 8-bit grayscale)
    if (bpp != 24 && bpp != 8) {
        fprintf(stderr, "Error: Unsupported BMP format (only 24-bit or 8-bit grayscale supported)\n");
        fclose(file);
        return -1;
    }

    // Calculate the total size of the image data
    int row_size = ((*width * bpp + 31) / 32) * 4; // Row size including padding
    int img_data_size = row_size * (*height);

    // Allocate memory for the image data
    *img = (unsigned char*)malloc(img_data_size);
    if (*img == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return -1;
    }

    // Read the pixel data
    fseek(file, *(int*)&header[10], SEEK_SET);  // Go to the pixel data offset
    fread(*img, 1, img_data_size, file);
    fclose(file);

    // If the image is grayscale (8-bit), convert it to RGB
    if (bpp == 8) {
        unsigned char* temp_img = (unsigned char*)malloc(*width * *height * 3);  // RGB buffer
        if (temp_img == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for grayscale to RGB conversion\n");
            free(*img);
            return -1;
        }

        // Convert grayscale to RGB
        for (int i = 0, j = 0; i < *height; i++) {
            for (int k = 0; k < *width; k++) {
                unsigned char gray = (*img)[i * row_size + k];
                temp_img[j++] = gray;  // Red
                temp_img[j++] = gray;  // Green
                temp_img[j++] = gray;  // Blue
            }
        }

        // Free the old image data and point to the new RGB data
        free(*img);
        *img = temp_img;
    }

    return 0;
}

// Function to resize an image using bilinear interpolation
void resize_image(unsigned char* input_img, unsigned char* output_img,
    int input_width, int input_height,
    int output_width, int output_height) {

    float x_ratio = (float)(input_width - 1) / (output_width - 1);
    float y_ratio = (float)(input_height - 1) / (output_height - 1);

    for (int y = 0; y < output_height; y++) {
        for (int x = 0; x < output_width; x++) {
            float gx = x * x_ratio;
            float gy = y * y_ratio;

            int x0 = (int)gx;
            int y0 = (int)gy;
            int x1 = (x0 + 1 < input_width) ? x0 + 1 : x0;
            int y1 = (y0 + 1 < input_height) ? y0 + 1 : y0;

            float wx = gx - x0;
            float wy = gy - y0;

            // Pixel values at the four corners
            unsigned char TL = input_img[y0 * input_width + x0];
            unsigned char TR = input_img[y0 * input_width + x1];
            unsigned char BL = input_img[y1 * input_width + x0];
            unsigned char BR = input_img[y1 * input_width + x1];

            // Bilinear interpolation
            float top = (1 - wx) * TL + wx * TR;
            float bottom = (1 - wx) * BL + wx * BR;
            float value = (1 - wy) * top + wy * bottom;

            output_img[y * output_width + x] = (unsigned char)(value + 0.5f);  // Round to nearest
        }
    }
}

void normalize_image(unsigned char* input_img, float* output_img, int output_width, int output_height) {
    for (int i = 0; i < output_width * output_height; i++) {
        output_img[i] = input_img[i] / 255.0f;  // Normalize to [0, 1]
    }
}


void preprocess_image(unsigned char* input_img, unsigned char* output_img, int input_width, int input_height, int output_width, int output_height) {
    unsigned char* resized_img = (unsigned char*)malloc(output_width * output_height * sizeof(unsigned char));
    resize_image(input_img, resized_img,input_width, input_height, output_width, output_height);
    normalize_image(resized_img, output_img, output_width, output_height);
    free(resized_img);
}

// Function to load an ONNX model and create an ONNX Runtime session
int load_model(const OrtApi* g_ort, const ORTCHAR_T* model_path, OrtEnv** out_env, OrtSession** out_session) {
    if (g_ort == NULL || model_path == NULL || out_env == NULL || out_session == NULL) {
        fprintf(stderr, "Invalid input parameters.\n");
        return -1;
    }

    OrtEnv* env = NULL;
    OrtSessionOptions* session_options = NULL;

    // ONNX Runtime 환경 생성
    ORT_ABORT_ON_ERROR(g_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntime", &env), g_ort);

    // 세션 옵션 생성
    ORT_ABORT_ON_ERROR(g_ort->CreateSessionOptions(&session_options), g_ort);

    // 세션 옵션 최적화 설정
    g_ort->SetSessionGraphOptimizationLevel(session_options, ORT_ENABLE_ALL);

    // 모델 로드 및 세션 생성
    ORT_ABORT_ON_ERROR(g_ort->CreateSession(env, model_path, session_options, out_session), g_ort);

    printf("ONNX model '%s' loaded successfully.\n", model_path);

    // 리소스 해제
    g_ort->ReleaseSessionOptions(session_options);

    *out_env = env;
    return 0;  // 성공
}


int run_model(const OrtApi* g_ort, OrtSession* session, float* input_data1, size_t input_size1,
    float* input_data2, size_t input_size2, float* output_data1, size_t output_size1,
    float* output_data2, size_t output_size2) {
    if (g_ort == NULL || session == NULL || input_data1 == NULL || input_data2 == NULL ||
        output_data1 == NULL || output_data2 == NULL) {
        fprintf(stderr, "Invalid input parameters.\n");
        return -1;
    }

    OrtMemoryInfo* memory_info;
    ORT_ABORT_ON_ERROR(g_ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &memory_info), g_ort);

    // 첫 번째 입력 텐서 생성
    int64_t input_shape1[] = { 1, 3, 224, 224 };
    OrtValue* input_tensor1 = NULL;
    ORT_ABORT_ON_ERROR(g_ort->CreateTensorWithDataAsOrtValue(memory_info, input_data1, input_size1 * sizeof(float),
        input_shape1, 4, ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &input_tensor1), g_ort);

    // 두 번째 입력 텐서 생성
    int64_t input_shape2[] = { 1, 3, 224, 224 };
    OrtValue* input_tensor2 = NULL;
    ORT_ABORT_ON_ERROR(g_ort->CreateTensorWithDataAsOrtValue(memory_info, input_data2, input_size2 * sizeof(float),
        input_shape2, 4, ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &input_tensor2), g_ort);

    // 모델 실행
    const char* input_names[] = { "input1", "input2" };
    const char* output_names[] = { "output1", "output2" };
    OrtValue* output_tensors[2] = { NULL, NULL };

    ORT_ABORT_ON_ERROR(g_ort->Run(session, NULL, input_names,
        (const OrtValue* const []) {
        input_tensor1, input_tensor2
    }, 2,
        output_names, 2, output_tensors), g_ort);

    // 첫 번째 출력 데이터 가져오기
    float* output_tensor_data1 = NULL;
    ORT_ABORT_ON_ERROR(g_ort->GetTensorMutableData(output_tensors[0], (void**)&output_tensor_data1), g_ort);
    for (size_t i = 0; i < output_size1; i++) {
        output_data1[i] = output_tensor_data1[i];
    }

    // 두 번째 출력 데이터 가져오기
    float* output_tensor_data2 = NULL;
    ORT_ABORT_ON_ERROR(g_ort->GetTensorMutableData(output_tensors[1], (void**)&output_tensor_data2), g_ort);
    for (size_t i = 0; i < output_size2; i++) {
        output_data2[i] = output_tensor_data2[i];
    }

    // 리소스 해제
    g_ort->ReleaseMemoryInfo(memory_info);
    g_ort->ReleaseValue(input_tensor1);
    g_ort->ReleaseValue(input_tensor2);
    g_ort->ReleaseValue(output_tensors[0]);
    g_ort->ReleaseValue(output_tensors[1]);

    return 0;  // 성공
}


// API function
int generate_template(const char* image_filename, const char* model_filename, float* input_template) {

    unsigned char* img = NULL;
    int width, height;

    // Read the image file
    if (read_bmp_image(image_filename, &img, &width, &height) != 0) {
        return -1;  // Error reading image
    }

    // Preprocess image
    unsigned char output_img[224 * 224];  // Preprocessed image (224x224)
    preprocess_image(img, output_img, width, height, 224, 224);
    free(img);

    // load_model();
    // run_model();
    
    return 0;

}
