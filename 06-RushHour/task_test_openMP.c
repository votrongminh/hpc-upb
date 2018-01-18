#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>



#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}
void fun1(int tab[], int pocz, int kon, int threadsLeft)
{
    if (threadsLeft <= 1) {
        for (int i = pocz; i < kon; i++)
            tab[i] = 2; // should make this constant something else to be more helpful
    }
    else
    {
        #pragma omp task
        fun1(tab, pocz, kon/2, threadsLeft/2);
        #pragma omp task
        fun1(tab, kon - kon/2, kon, threadsLeft - threadsLeft/2);
        #pragma omp taskwait
    }
}

int main()
{

    int N = 2000000000;
	double start, finish;
	
    int* tab = new int[N];
    for (int i = 0; i < N; i++)
        tab[i] = 0;

	GET_TIME(start);
    #pragma omp parallel
    // Only the first thread will spawn other threads
    #pragma omp single nowait
    {
        int allThreads = omp_get_num_threads();
        fun1(tab, 0, N, allThreads);
    }
	GET_TIME(finish);
	
	printf("\nExecution of Check() take: %e seconds\n", finish - start);

}