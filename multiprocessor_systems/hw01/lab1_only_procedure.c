#include<stdio.h>
#include<stdlib.h>
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
	printf("Array size = %ld, result of fourth method = %.2f, ticks = %ld\n", n, result, rdtsc() - tick);
	
	free(vec1);
	vec1 = NULL;
	free(vec2);
	vec2 = NULL;
}
