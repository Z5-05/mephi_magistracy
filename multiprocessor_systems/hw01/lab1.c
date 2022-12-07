#include<stdio.h>
#include<stdlib.h>
#include <immintrin.h>
#include <x86intrin.h>

#pragma intrinsic(__rdtsc)

u_int64_t rdtsc()
{
     return __rdtsc();
}

float procedure_programming(const float* vec1, const float* vec2, size_t n)
{
    float sum = 0;
    for (size_t i = 0; i < n; ++i)
        sum += vec1[i] * vec2[i];
    return sum;
}

float intrinsics_operations(const float* vec1, const float* vec2, size_t n)
{   
    float sum = 0;
    size_t i, j;
    __m256 acc = _mm256_setzero_ps();
    __m256 mul_sum, shuffle_sum;
    __m256 a, b;
    for(i = 0 ; i < n; i += 8 )
    {
          a = _mm256_loadu_ps( &vec1[i] );
          b = _mm256_loadu_ps( &vec2[i] );
          
          mul_sum = _mm256_mul_ps( a, b );
          shuffle_sum = _mm256_permute_ps( mul_sum, 0b10110001 );
          mul_sum = _mm256_add_ps( mul_sum, shuffle_sum );
          shuffle_sum = _mm256_permute_ps( mul_sum, 0b00011011 );
          mul_sum = _mm256_add_ps( mul_sum, shuffle_sum );
          shuffle_sum = _mm256_permute2f128_ps(mul_sum, mul_sum, 0b00000001);
          mul_sum = _mm256_add_ps( mul_sum, shuffle_sum );
          float* f = (float*)&mul_sum;
          sum += mul_sum[0];
        }
    return sum;
}

float gcc_vector_extensions(const float* vec1, const float* vec2, size_t n)
{  
    typedef int v8si __attribute__ ((vector_size(32)));	
    typedef float v8sa __attribute__ ((vector_size(32)));
    size_t i, j;
    float sum = 0;
    v8sa results = {0, 0, 0, 0, 0, 0, 0, 0};
    v8sa tmp_res;
    v8si mask1 = {1, 0, 3, 2, 5, 4, 7, 6};
    v8si mask2 = {2, 2, 0, 0, 6, 6, 4, 4};
    v8si mask3 = {4, 4, 4, 4, 0, 0, 0, 0};
    for(i = 0 ; i < n; i += 8)
    {
        v8sa vector1 = {vec1[i], vec1[i+1], vec1[i+2], vec1[i+3], vec1[i+4], vec1[i+5], vec1[i+6], vec1[i+7]};
        v8sa vector2 = {vec2[i], vec2[i+1], vec2[i+2], vec2[i+3], vec2[i+4], vec2[i+5], vec2[i+6], vec2[i+7]};
        results += vector1 * vector2;
    }
    tmp_res = __builtin_shuffle(results, mask1);
    results += tmp_res;
    tmp_res = __builtin_shuffle(results, mask2);
    results += tmp_res;
    tmp_res = __builtin_shuffle(results, mask3);
    results += tmp_res;

    return results[0];
}

int main(int argc, char **argv)
{
    size_t n;
    size_t i;
    if (argc < 2)
        exit(1);
    n = atoi(argv[1]);
    float result;
    u_int64_t tick;

    float* vec1 = (float*)malloc(n * (n / 8 + 1) * sizeof(float));
    float* vec2 = (float*)malloc(n * (n / 8 + 1) * sizeof(float));
    
    for (i = 0; i < n * (n / 8 + 1); ++i)
    {
        *(vec1 + i) = 0;
        *(vec2 + i) = 0;
    }
        
    for (i = 0; i < n; ++i)
    {
        vec1[i] = i + 1;
        vec2[i] = i + 1;
    }

    tick = rdtsc();
    result = procedure_programming(vec1, vec2, n);
    printf("Array size = %ld, result of first method = %.2f, ticks = %ld\n", n, result, rdtsc() - tick);
    tick = rdtsc();
    result = intrinsics_operations(vec1, vec2, n);
    printf("Array size = %ld, result of second method = %.2f, ticks = %ld\n", n, result, rdtsc() - tick);
    tick = rdtsc();
    result = gcc_vector_extensions(vec1, vec2, n);
    printf("Array size = %ld, result of third method = %.2f, ticks = %ld\n", n, result, rdtsc() - tick);
    
    free(vec1);
    vec1 = NULL;
    free(vec2);
    vec2 = NULL;
    }
