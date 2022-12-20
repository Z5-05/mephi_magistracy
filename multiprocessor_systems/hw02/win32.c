#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
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
    result = parallel_win32(vec1, vec2, n);
    time_end = clock();
    printf("Array size = %ld, result of posix = %.2f, time (ms) = %f\n", n, result, (double)(time_end - time_begin) / CLOCKS_PER_SEC);
    
    free(vec1);
    vec1 = NULL;
    free(vec2);
    vec2 = NULL;
    return 0;
    }


double parallel_win32(double* vec1, double* vec2, size_t n)
{
    PVARIABLES pDataArray[N_THREADS];
    DWORD   dwThreadIdArray[N_THREADS];
    HANDLE  hThreadArray[N_THREADS]; 

    for( int i=0; i<N_THREADS; i++ )
    {
        pDataArray[i] = (pvariable) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(pvariable));

        if(pDataArray[i] == NULL)
            ExitProcess(2);

        pDataArray[i].idx = i;
        pDataArray[i].result = 0;
        pDataArray[i].sub_size = n/N_THREADS + 1;
        pDataArray[i].vec1 = vec1;
        pDataArray[i].vec2 = vec2;
        pDataArray[i].n = n;

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

    for(int i=0; i<N_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        if(pDataArray[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
            pDataArray[i] = NULL;   
        }
    }

    for(i=0; i < N_THREADS; ++i)
        result += array_ptr[i].result;
   return result;
}

DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
    HANDLE hStdout;
    PVARIABLES pDataArray;
    pDataArray = (PVARIABLES)lpParam;

    double result = 0;
    for (i=ptr->idx * ptr->sub_size; i < MIN(ptr->n, (ptr->idx + 1)*ptr->sub_size); ++i)
        result += ptr->vec1[i] * ptr->vec2[i];
    ptr->result = result;

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