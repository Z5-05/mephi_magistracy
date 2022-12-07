
#include <stdio.h>
#include <pthread.h>
#include <x86intrin.h>

#pragma intrinsic(__rdtsc)

float* vec1;
float* vec2;
size_t n;
float* partial_sum;
void *dot_function_for_posix(void *ptr);


u_int64_t rdtsc()
{
     return __rdtsc();
}

float parallel_posix(const float* vec1, const float* vec2, size_t n)
{
   pthread_t thread_id[n];
   size_t i;
   float result = 0;

   for(i=0; i < n; ++i)
   {    
      size_t* p;
      p = malloc(sizeof(size_t));
      *p = i;
      pthread_create( &thread_id[i], NULL, dot_function_for_posix, p );
   }

   for(i=0; i < n; ++i)
   {
      pthread_join( thread_id[i], NULL); 
   }
   for(i=0; i < n; ++i)
        result += partial_sum[i];
   return result;
}

void *dot_function_for_posix(void *arg)
{
    size_t idx = *(size_t*)arg;
    free(arg);
    arg = NULL;

    partial_sum[idx] = vec1[idx] * vec2[idx];
    return (NULL);
};

int main(int argc, char **argv)
{   
    size_t n;
    size_t i;
    if (argc < 2)
        exit(1);
    
    n = atoi(argv[1]);
    float result;
    u_int64_t tick;

    vec1 = (float*)malloc(n * sizeof(float));
    vec2 = (float*)malloc(n * sizeof(float));
    partial_sum = (float*)malloc(n * sizeof(float));
    
    
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
    result = parallel_posix(vec1, vec2, n);
    u_int64_t tick_end = rdtsc();
    clock_t time_end = clock();
    printf("Array size = %ld, result of procedure = %.2f, ticks = %ld, time (ms) = %f\n", n, result, tick_end-tick_begin, (double)(time_end - time_begin) / CLOCKS_PER_SEC);
    
    free(vec1);
    vec1 = NULL;
    free(vec2);
    vec2 = NULL;
    return 0;
    }