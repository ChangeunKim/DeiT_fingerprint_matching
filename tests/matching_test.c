#include "../matching.h"

void test_cosine_similarity() {
	float vector1[] = { 1.0, 1.0 };
	float vector2[] = { 0, 5.0 };
	int vector_length = 2;

	float similarity = cosine_similarity(vector1, vector2, vector_length);
	printf("Cosine Similarity: %.6f\n", similarity);

	return 0;
}
