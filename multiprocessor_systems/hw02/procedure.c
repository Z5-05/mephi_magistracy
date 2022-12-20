#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double procedure_programming(double*, double*, size_t);


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
    result = procedure_programming(vec1, vec2, n);
    time_end = clock();
    printf("Array size = %ld, result of procedure = %.2f, time (ms) = %f\n", n, result, (double)(time_end - time_begin) / CLOCKS_PER_SEC);
    
    free(vec1);
    vec1 = NULL;
    free(vec2);
    vec2 = NULL;
    return 0;
    }

double procedure_programming(double* vec1, double* vec2, size_t n)
{
    double sum = 0;
    for (size_t i = 0; i < n; ++i)
        sum += vec1[i] * vec2[i];
    return sum;
}