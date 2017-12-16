/* 
gcc -g -Wall -fopenmp -o openMP_GaussianElimination_RowOriented openMP_GaussianElimination_RowOriented.c
./openMP_GaussianElimination_RowOriented 1

 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void openMP_GaussianElimination_RowOriented(double A[3][3], double *b, double *x); /* Thread function */
void openMP_GaussianElimination_RowOriented_openMP(double A[3][3], double *b, double *x); /* Thread function */

int main(int argc, char* argv[]) {
/* Get number of threads from command line */
int thread_count = strtol(argv[1], NULL, 10);
int i;

double x[3];
double A[3][3] = {  
   {2, -3, 0} ,   /*  initializers for row indexed by 0 */
   {0, 1, 1} ,   /*  initializers for row indexed by 1 */
   {0, 0, -5}   /*  initializers for row indexed by 2 */
};
double b[3] = {3, 1, 0};

# pragma omp parallel num_threads(thread_count)
openMP_GaussianElimination_RowOriented(A, &b, &x);

for (i =0; i<3; i++) {
	printf("X[%d] is having value: %2.2f \n", i, x[i]);
}
return 0;
} /* main */

void openMP_GaussianElimination_RowOriented(double A[3][3], double*b, double*x) {
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();

	//printf("Hello from thread %d of %d\n", my_rank, thread_count);

	int n = 3;
	int col, row, i;
	// double x[3];
	// double A[3][3] = {  
	   // {2, -3, 0} ,   /*  initializers for row indexed by 0 */
	   // {0, 1, 1} ,   /*  initializers for row indexed by 1 */
	   // {0, 0, -5}   /*  initializers for row indexed by 2 */
	// };
	// double b[3] = {3, 1, 0};

	for (row = n-1; row >= 0; row--) {
		x[row] = b[row];
		printf("loop through row:%d \n", row);
		for (col = row+1; col < n; col++) {
			x[row] -= A[row][col]*x[col];
			printf("loop through col:%d \n", col);
		}
		x[row] /= A[row][row];
	}
	for (i =0; i<3; i++) {
		printf("X[%d] inside loop is having value: %2.2f \n", i, x[i]);
	}

} /* openMP_GaussianElimination_RowOriented */


void openMP_GaussianElimination_RowOriented_openMP(double A[3][3], double*b, double*x) {
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();

	//printf("Hello from thread %d of %d\n", my_rank, thread_count);

	int n = 3;
	int col, row, i;
	// double x[3];
	// double A[3][3] = {  
	   // {2, -3, 0} ,   /*  initializers for row indexed by 0 */
	   // {0, 1, 1} ,   /*  initializers for row indexed by 1 */
	   // {0, 0, -5}   /*  initializers for row indexed by 2 */
	// };
	// double b[3] = {3, 1, 0};

	for (row = n-1; row >= 0; row--) {
		x[row] = b[row];
		printf("loop through row:%d \n", row);
		for (col = row+1; col < n; col++) {
			x[row] -= A[row][col]*x[col];
			printf("loop through col:%d \n", col);
		}
		x[row] /= A[row][row];
	}
	for (i =0; i<3; i++) {
		printf("X[%d] inside loop is having value: %2.2f \n", i, x[i]);
	}

} /* openMP_GaussianElimination_RowOriented_openMP */