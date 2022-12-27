#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
using namespace std;
#define MIN(a,b) (((a)<(b))?(a):(b))
#define N_THREADS 4

double parallel_posix(double*, double*, size_t);
void *dot_function_for_posix(void *ptr);

typedef struct Variables
{
    size_t idx;
    size_t sub_size;
    double result;
    double* vec1;
    double* vec2;
    size_t n;
} variables;

int main(int argc, char **argv)
{   
    double* vec1;
    double* vec2;
    size_t n, i;
    double result;
    chrono::steady_clock::time_point time_end;
    chrono::steady_clock::time_point time_begin;

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

    time_begin = chrono::steady_clock::now();
    result = parallel_posix(vec1, vec2, n);
    time_end = chrono::steady_clock::now();
    printf("Array size = %ld, result of posix = %.2f, time (ms) = %lu\n", n, result, chrono::duration_cast<chrono::nanoseconds>(time_end - time_begin).count());
    
    delete vec1;
    vec1 = NULL;
    delete vec2;
    vec2 = NULL;
    return 0;
    }

double parallel_posix(double* vec1, double* vec2, size_t n)
{
   pthread_t thread_id[N_THREADS];
   variables array_ptr[N_THREADS];
   size_t i;
   double result;
   result = 0;

   for(i=0; i < N_THREADS; ++i)
   {    
      array_ptr[i].idx = i;
      array_ptr[i].result = 0;
      array_ptr[i].sub_size = n/N_THREADS + 1;
      array_ptr[i].vec1 = vec1;
      array_ptr[i].vec2 = vec2;
      array_ptr[i].n = n;
      pthread_create( &thread_id[i], NULL, dot_function_for_posix, &array_ptr[i]);
   }

   for(i=0; i < N_THREADS; ++i)
   {
      pthread_join(thread_id[i], NULL); 
   }
   for(i=0; i < N_THREADS; ++i)
        result += array_ptr[i].result;
   return result;
}

void *dot_function_for_posix(void *arg)
{   
    size_t i;
    variables* ptr;
    ptr = (variables*)arg;

    double result = 0;
    for (i=ptr->idx * ptr->sub_size; i < MIN(ptr->n, (ptr->idx + 1)*ptr->sub_size); ++i)
        result += ptr->vec1[i] * ptr->vec2[i];
    ptr->result = result;
    return (NULL);
}
