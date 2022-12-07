
#include <stdio.h>
#include <time.h>
#include <x86intrin.h>

#pragma intrinsic(__rdtsc)

float* vec1;
float* vec2;
size_t n;

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

int main(int argc, char **argv)
{
    size_t i;
    if (argc < 2)
        exit(1);
    n = atoi(argv[1]);
    float result;

    vec1 = (float*)malloc(n * sizeof(float));
    vec2 = (float*)malloc(n * sizeof(float));
    
    for (i = 0; i < n; ++i)
    {
        *(vec1 + i) = 0;
        *(vec2 + i) = 0;
    }
        
    for (i = 0; i < n; ++i)
    {
        vec1[i] = i + 1;
        vec2[i] = i + 1;
    }

    clock_t time_begin = clock();
    u_int64_t tick_begin = rdtsc();
    result = procedure_programming(vec1, vec2, n);
    u_int64_t tick_end = rdtsc();
    clock_t time_end = clock();
    printf("Array size = %ld, result of procedure = %.2f, ticks = %ld, time (ms) = %f\n", n, result, tick_end-tick_begin, (double)(time_end - time_begin) / CLOCKS_PER_SEC);
    
    free(vec1);
    vec1 = NULL;
    free(vec2);
    vec2 = NULL;
    return 0;
    }