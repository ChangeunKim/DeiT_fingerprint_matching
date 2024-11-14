#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_bmp_reader();
void test_cosine_similarity();

int main() {
    printf("Running test: BMP Reader\n");
    test_bmp_reader();
    printf("Completed test: BMP Reader\n\n");

    printf("Running test: Cosine Similarity\n");
    test_cosine_similarity();
    printf("Completed test: Cosine Similarity\n");

    printf("\nAll tests completed.\n");
    return 0;
}
