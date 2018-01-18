/*   
	gcc -g -Wall -fopenmp -o sumArray sumArray.c
	./sumArray
*/
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

float sum(const float *, size_t);

#define N 100000000  // we'll sum this many numbers

int main()
{
    float *a = malloc(N * sizeof(float));
	double start, finish;
    if (a == NULL) {
        perror("malloc");
        return 1;
    }

    // fill the array a
    for (size_t i = 0; i < N; i++) {
        a[i] = .000001;
    }
	
	GET_TIME(start);
	
    printf("%f\n", sum(a, N));
	GET_TIME(finish);
	
	printf("\nExecution of Check() take: %e seconds\n", finish - start);
	
    return 0;

}

float sum(const float *a, size_t n)
{
    float total = 0;
    size_t i;
	#pragma omp parallel for reduction(+:total)
    for (i = 0; i < n; i++) {
        total += a[i];
    }
    return total;
}