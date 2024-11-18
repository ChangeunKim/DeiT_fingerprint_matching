#include "../matching.h"
#include "../template.h"

void test_cosine_similarity() {
	float vector1[] = { 1.0, 1.0 };
	float vector2[] = { 0, 5.0 };
	int vector_length = 2;

	float similarity = cosine_similarity(vector1, vector2, vector_length);
	printf("Cosine Similarity: %.6f\n", similarity);

	return 0;
}

void test_verification(const float* embed1, const float* embed2) {
	float distance = fingerprint_verification(embed1, embed2);

	printf("Distance between the two templates: %.4f\n\n", distance);
}

// Utility function to print scores for debugging
void print_scores(float* scores, int size) {
    for (int i = 0; i < size; ++i) {
        printf("Score %d: %f\n", i+1, scores[i]);
    }
}

// Testing function for fingerprint_identification
void test_identification(const ORTCHAR_T* model_path) {

    // Image filenames for the fingerprint database
    int db_size = 30;

    const char* image_filenames[30] = {
        "tests/samples/fingerprint_image.bmp",
        "tests/samples/fingerprint_image(1).bmp",
        "tests/samples/fingerprint_image(2).bmp",
        "tests/samples/fingerprint_image(3).bmp",
        "tests/samples/fingerprint_image(4).bmp",
        "tests/samples/fingerprint_image(5).bmp",
        "tests/samples/fingerprint_image(6).bmp",
        "tests/samples/fingerprint_image(7).bmp",
        "tests/samples/fingerprint_image(8).bmp",
        "tests/samples/fingerprint_image(9).bmp",
        "tests/samples/fingerprint_image(10).bmp",
        "tests/samples/fingerprint_image(11).bmp",
        "tests/samples/fingerprint_image(12).bmp",
        "tests/samples/fingerprint_image(13).bmp",
        "tests/samples/fingerprint_image(14).bmp",
        "tests/samples/fingerprint_image(15).bmp",
        "tests/samples/fingerprint_image(16).bmp",
        "tests/samples/fingerprint_image(17).bmp",
        "tests/samples/fingerprint_image(18).bmp",
        "tests/samples/fingerprint_image(19).bmp",
        "tests/samples/fingerprint_image(20).bmp",
        "tests/samples/fingerprint_image(21).bmp",
        "tests/samples/fingerprint_image(22).bmp",
        "tests/samples/fingerprint_image(23).bmp",
        "tests/samples/fingerprint_image(24).bmp",
        "tests/samples/fingerprint_image(25).bmp",
        "tests/samples/fingerprint_image(26).bmp",
        "tests/samples/fingerprint_image(27).bmp",
        "tests/samples/fingerprint_image(28).bmp",
        "tests/samples/fingerprint_image(29).bmp"
    };

    // Allocate memory for template database (db_size * 64 elements, assuming 64-dimensional embeddings)
    float** template_db = (float**)malloc(30 * sizeof(float*));
    if (template_db == NULL) {
        fprintf(stderr, "Memory allocation failed for template_db.\n");
        return;
    }
    for (int i = 0; i < db_size; ++i) {
        template_db[i] = (float*)malloc(64 * sizeof(float));
        if (template_db[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for template %d.\n", i);
            return;
        }
    }

    // Load model and session
    const OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    OrtEnv* env = NULL;
    OrtSession* session = NULL;

    if (load_model(g_ort, model_path, &env, &session) != 0) {
        fprintf(stderr, "Test failed: Failed to load model.\n");
        return;
    }

    // Generate templates for each image in the database
    for (int i = 0; i < db_size; ++i) {
        if (generate_template(image_filenames[i], g_ort, env, session, template_db[i]) != 0) {
            fprintf(stderr, "Failed to generate template for image: %s\n", image_filenames[i]);
            return;
        }
    }

    // Allocate memory for the score array to store the results of identification
    float* score = (float*)malloc(db_size * sizeof(float));
    if (score == NULL) {
        fprintf(stderr, "Memory allocation failed for score array.\n");
        return;
    }

    // Test fingerprint_identification by using each template as a query template
    for (int i = 0; i < db_size; ++i) {
        printf("Testing with template %d as query...\n", i + 1);

        // Perform identification (use the template_db[i] as query_template)
        if (fingerprint_identification(template_db[i], template_db, db_size, score) != 0) {
            fprintf(stderr, "Failed to identify fingerprint template %d.\n", i + 1);
            return;
        }

        // Print the scores for the identification test
        //print_scores(score, db_size);

        // Optionally, print the results to check if the query matches the correct template (self-matching should have the lowest score)
        printf("Query template %d vs. all templates:\n", i + 1);
        for (int j = 0; j < db_size; ++j) {
            if (i == j) {
                printf("Template %d matched itself with score: %f (expected to be the lowest)\n", i + 1, score[j]);
            }
        }
        printf("\n");
    }

    // Clean up
    for (int i = 0; i < db_size; ++i) {
        free(template_db[i]);
    }
    free(template_db);
    free(score);
    clean_model(g_ort, env, session);
}