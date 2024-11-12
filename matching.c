#include "matching.h"

float cosine_similarity(const float* vector1, const float* vector2) {

	return 0.0;
}

float template_distance(const float* template1, const float* template2) {

	float similarity = 0.0;

	similarity = cosine_similarity(template1, template2);

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