#ifndef MATCHING_H
#define MATCHING_H

#include <math.h>
#include "config.h"

float cosine_similarity(const float* vector1, const float* vector2, int vector_length);
float template_distance(const float* template1, const float* template2);

// API function
DllAPI int fingerprint_identification(float* query_template, float** template_db, int db_size, float* score);
DllAPI float fingerprint_verification(const float* query_template, const float* verify_template);

#endif // MATCHING_H