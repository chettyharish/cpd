#include <immintrin.h>
#include <stdio.h>

int main() {

	long int x[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	__m256i f1_vals = _mm256_load_si256((__m256i *)&x[0]);
	__m256i f2_vals = _mm256_load_si256((__m256i *)&x[4]);
	_mm256_min_ep64(f1_vals, f2_vals);
//	__m256d _mm_min_ps(__m128d a, __m128d b);
//
//	/* Display the elements of the result vector */
//	long int *ans = (long int *) &result;
//	printf("%ld %ld %ld %ld\n", ans[0], ans[1], ans[2], ans[3]);

	return 0;
}
