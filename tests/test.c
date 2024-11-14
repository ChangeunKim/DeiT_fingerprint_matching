#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_resize_image.h"
#include "../template.h"


void test_bmp_reader();
void test_cosine_similarity();

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
    

    printf("\nAll tests completed.\n");
    return 0;
}
