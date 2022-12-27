#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
using namespace std;

#define MAX_THREADS 4
#define MIN(a,b) (((a)<(b))?(a):(b))

DWORD WINAPI MyThreadFunction( LPVOID lpParam );
void ErrorHandler(LPCTSTR lpszFunction);

typedef struct Variables {
    double* vec1;
    double* vec2;
    size_t n;
    double idx;
    double sub_size;
    double result;
} VARIABLES, *PVARIABLES;

int _tmain(int argc, char **argv) {
    if (argc < 2)
        exit(1);
    size_t n = atoi(argv[1]);

    double* vec1 = (double*) malloc(n * sizeof(double));
    double* vec2 = (double*) malloc(n * sizeof(double));

    for (size_t i = 0; i < n; i++) {
        vec1[i] = (i + 1) % 100;
        vec2[i] = (i + 1) % 100;
    }
    PVARIABLES pDataArray[MAX_THREADS];
    DWORD dwThreadIdArray[MAX_THREADS];
    HANDLE hThreadArray[MAX_THREADS];

	chrono::steady_clock::time_point begin_time = chrono::steady_clock::now();
	for (size_t i = 0; i < MAX_THREADS; i++) {
      pDataArray[i] = (PVARIABLES) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VARIABLES));

      if (pDataArray[i] == NULL)
         ExitProcess(2);

      pDataArray[i]->idx = i;
      pDataArray[i]->result = 0;
      pDataArray[i]->sub_size = n/MAX_THREADS + 1;
      pDataArray[i]->vec1 = vec1;
      pDataArray[i]->vec2 = vec2;
      pDataArray[i]->n = n;

      hThreadArray[i] = CreateThread(NULL, 0, MyThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);

      if (hThreadArray[i] == NULL) {
         ErrorHandler(TEXT("CreateThread"));
         ExitProcess(3);
      }
   }

   WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

   double results = 0;
   for (size_t i = 0; i < MAX_THREADS; i++)
	   results += pDataArray[i]->result;
   chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
   printf("Array size = %ld, result of win32 = %.2f, time (ms) = %llu\n", n, results, chrono::duration_cast<chrono::nanoseconds>(end_time - begin_time).count());

   for (size_t i = 0; i < MAX_THREADS; i++) {
      CloseHandle(hThreadArray[i]);
      if (pDataArray[i] != NULL) {
         HeapFree(GetProcessHeap(), 0, pDataArray[i]);
         pDataArray[i] = NULL;
      }
   }  
  
   return 0;
}

DWORD WINAPI MyThreadFunction( LPVOID lpParam ) {
    HANDLE hStdout;
    PVARIABLES pDataArray;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if ( hStdout == INVALID_HANDLE_VALUE )
       return 1;

    pDataArray = (PVARIABLES)lpParam;

    for (size_t i=pDataArray->idx * pDataArray->sub_size; i < MIN(pDataArray->n, (pDataArray->idx + 1)*pDataArray->sub_size); ++i)
        pDataArray->result += pDataArray->vec1[i] * pDataArray->vec2[i];

    return 0;
}

void ErrorHandler(LPCTSTR lpszFunction) {
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
      0, NULL);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK);

   LocalFree(lpMsgBuf);
   LocalFree(lpDisplayBuf);
}


