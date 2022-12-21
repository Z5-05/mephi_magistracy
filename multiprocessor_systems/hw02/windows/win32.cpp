#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <fstream>
#include <chrono>
using namespace std;
#define MIN(a,b) (((a)<(b))?(a):(b))
#define N_THREADS 4

DWORD WINAPI MyThreadFunction( LPVOID lpParam );
void ErrorHandler(LPCTSTR lpszFunction);
double parallel_win32(double*, double*, size_t);

typedef struct Variables
{
    size_t idx;
    size_t sub_size;
    double result;
    double* vec1;
    double* vec2;
    size_t n;
} VARIABLES, *PVARIABLES;

int _tmain(int argc, char **argv)
{   
    double* vec1;
    double* vec2;
    size_t n;
    size_t i;
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
    result = parallel_win32(vec1, vec2, n);
    time_end = chrono::steady_clock::now();
    printf("Array size = %ld, result of win32 = %.2f, time (ms) = %llu\n", n, result, chrono::duration_cast<chrono::nanoseconds>(time_end - time_begin).count());
    free(vec1);
    vec1 = NULL;
    free(vec2);
    vec2 = NULL;
    
    return 0;
    }


double parallel_win32(double* vec1, double* vec2, size_t n)
{
    size_t i;
    double results;
    PVARIABLES pDataArray[N_THREADS];
    DWORD   dwThreadIdArray[N_THREADS];
    HANDLE  hThreadArray[N_THREADS]; 
     
    for( int i=0; i<N_THREADS; i++ )
    {
        pDataArray[i] = (PVARIABLES) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(PVARIABLES));
        
        if(pDataArray[i] == NULL)
            ExitProcess(2);
        pDataArray[i]->idx = i;
        pDataArray[i]->result = 0;
        pDataArray[i]->sub_size = n/N_THREADS + 1;
        pDataArray[i]->vec1 = vec1;
        pDataArray[i]->vec2 = vec2;
        pDataArray[i]->n = n;

        hThreadArray[i] = CreateThread( 
            NULL,
            0,    
            MyThreadFunction,
            pDataArray[i],  
            0,        
            &dwThreadIdArray[i]); 

        if (hThreadArray[i] == NULL) 
        {
           ErrorHandler(TEXT("CreateThread"));
           ExitProcess(3);
        }
    }
    WaitForMultipleObjects(N_THREADS, hThreadArray, TRUE, INFINITE);
    for(i=0; i < N_THREADS; ++i)
        results += pDataArray[i]->result;
   return results;
    for(int i=0; i<N_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        if(pDataArray[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
            pDataArray[i] = NULL;   
        }
    }
   
}

DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
    size_t i;
    HANDLE hStdout;
    PVARIABLES pDataArray;
    pDataArray = (PVARIABLES)lpParam;

    double result = 0;
    for (i=pDataArray->idx * pDataArray->sub_size; i < MIN(pDataArray->n, (pDataArray->idx + 1)*pDataArray->sub_size); ++i)
        result += pDataArray->vec1[i] * pDataArray->vec2[i];
    pDataArray->result = result;

    return 0; 
} 

void ErrorHandler(LPCTSTR lpszFunction) 
{ 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );


    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}