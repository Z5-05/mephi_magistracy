
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define MIN(a,b) (((a)<(b))?(a):(b))
#define N_THREADS 4

double parallel_openmp(double*, double*, size_t);
double dot_function_for_openmp(double*, double*, size_t, size_t, size_t);

int main(int argc, char **argv)
{   
    double* vec1;
    double* vec2;
    size_t n, i;
    double result;
    clock_t time_begin, time_end;

    if (argc < 2)
        exit(1);
    
    n = atoi(argv[1]);

    vec1 = (double*)malloc(n * sizeof(double));
    vec2 = (double*)malloc(n * sizeof(double));
        
    for (i = 0; i < n; ++i)
    {
        vec1[i] = (i + 1) % 100;
        vec2[i] = (i + 1) % 100;
    }

    time_begin = clock();
    result = parallel_openmp(vec1, vec2, n);
    time_end = clock();
    printf("Array size = %ld, result of openmp = %.2f, time (ms) = %f\n", n, result, (double)(time_end - time_begin) / CLOCKS_PER_SEC);
    
    free(vec1);
    vec1 = NULL;
    free(vec2);
    vec2 = NULL;
    return 0;
    }

double parallel_openmp(double* vec1, double* vec2, size_t n)
{
   double results = 0;
   size_t i;
   size_t sub_size = n / N_THREADS + 1;

   omp_set_num_threads(N_THREADS);
   #pragma omp parallel private(i)
   {
        #pragma omp for reduction(+ : results)
        for (i = 0; i <N_THREADS; ++i)
            results += dot_function_for_openmp(vec1, vec2, n, i, sub_size);
   }
    return results;
}

double dot_function_for_openmp(double* vec1, double* vec2, size_t n, size_t idx, size_t sub_size)
{   
    size_t i;
    double result = 0;
    for (i=idx * sub_size; i < MIN(n, (idx + 1)*sub_size); ++i)
        result += vec1[i] * vec2[i];
    return result;
}
