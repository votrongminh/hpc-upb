/* 
gcc -g -Wall -fopenmp -o openMP_GaussianElimination openMP_GaussianElimination.c
./openMP_GaussianElimination 1

 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void openMP_GaussianElimination_RowOriented(double A[3][3], double *b, double *x); /* Thread function */
void openMP_GaussianElimination_RowOriented_openMP(double A[3][3], double *b, double *x); /* Thread function */

int main(int argc, char* argv[]) {
/* Get number of threads from command line */
int thread_count = strtol(argv[1], NULL, 10);
int i, j, k;

double x[3];
double A[3][3] = {  
   {2, -3, 0} ,   /*  initializers for row indexed by 0 */
   {0, 1, 1} ,   /*  initializers for row indexed by 1 */
   {0, 0, -5}   /*  initializers for row indexed by 2 */
};
double b[3] = {3, 1, 0};

int N =3;
double ratio;

/* Gaussian Elimination (no pivoting): x = A\b */ 
for (i = 0; i < N-1; i++) {
  for (j = i; j < N; j++) {
    ratio = A[j][i]/A[i][i];
    for (k = i; k < N; k++) {
      A[j][k] -= (ratio*A[i][k]);
      b[j] -= (ratio*b[i]);
    }
  }
}


for (i =0; i<3; i++) {
	printf("X[%d] is having value: %2.2f \n", i, b[i]);
}
return 0;
} /* main */
