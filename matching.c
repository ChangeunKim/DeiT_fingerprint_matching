#include "matching.h"
#include <math.h>
#include <stdio.h>

float cosine_similarity(const float* vector1, const float* vector2, int vector_length) {
	float dot_product = 0.0;
	float magnitude1 = 0.0;
	float magnitude2 = 0.0;

	// Calculate dot product and magnitudes
	for (int i = 0; i < vector_length; ++i) {
		dot_product += vector1[i] * vector2[i];
		magnitude1 += vector1[i] * vector1[i];
		magnitude2 += vector2[i] * vector2[i];
	}
	magnitude1 = sqrtf(magnitude1);
	magnitude2 = sqrtf(magnitude2);

	// Avoid division by zero
	if (magnitude1 == 0 || magnitude2 == 0) {
		return 0.0;
	}

	return dot_product / (magnitude1 * magnitude2);
}

float template_distance(const float* template1, const float* template2) {

	float similarity = 0.0;
	int vector_length = 64;

	similarity = cosine_similarity(template1, template2, vector_length);

	return 1.0 - similarity;
}

// API function	
int fingerprint_identification(float* query_template, float** template_db, int db_size, float* score) {

	for (int i = 0; i < db_size; i++) {
		score[i] = template_distance(query_template, template_db[i]);
	}

	return 0;
}

float fingerprint_verification(const float* query_template, const float* verify_template) {
	return template_distance(query_template, verify_template);
}
