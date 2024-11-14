#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_functions.h"
#include "../template.h"


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
    const char* model_path = "tests/optimized_deit_tiny_siamese.onnx";// "C: / Users / owner / source / repos / DeiT_fingerprint_matching / tests / optimized_deit_tiny_siamese.onnx";
    OrtSession* session = NULL;
    OrtEnv* env = NULL;

    // Initialize ONNX Runtime
    const OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (!g_ort) {
        fprintf(stderr, "Failed to initialize ONNX Runtime.\n");
        return -1;
    }

    // Load the model
    if (load_model(g_ort, model_path, &session, &env) != 0) {
        return -1;
    }

    // Prepare dummy input data
    float input_template[1 * 3 * 224 * 224] = { 0.0f };


    // Clean up
    g_ort->ReleaseSession(session);
    g_ort->ReleaseEnv(env);

    return 0;
}
