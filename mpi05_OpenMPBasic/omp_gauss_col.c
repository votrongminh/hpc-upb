/* File:     
 *     omp_gauss_col.c 
 *
 *
 * Purpose:  
 *          Solves an upper triangular system when the matrice A[n][n] is provided,
 *          together with the "right-hand side" solutions into the array B[n]. 
 *          The solutions of the system will be recorded in the array X[n].
 *
 * Compile:  
 *    gcc -g -Wall -fopenmp -o omp_gauss_col omp_gauss_col.c 
 * Usage:
 *    ./omp_gauss_col <thread_count> <matrix/array dimensions>
 *    Eg: ./omp_gauss_col 8 10
 * Runtime schedule types:
 *    export OMP_SCHEDULE="static,1"
 *    export OMP_SCHEDULE="dynamic,1"
 *    export OMP_SCHEDULE="guided,1"
 */


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


/* Serial functions */
void Get_args(int argc, char* argv[], int* thread_count_p, int* n_p);
void Usage(char* prog_name);
void Gen_matrix(double A[], int n);
void Gen_vector(double x[], int n);
void Print_matrix(char* title, double y[], int n);
void Print_vector(char* title, double y[], double n);

/* Parallel functions */
void Gauss_col (double A[], double B[], double X[], int n, int thread_count);


int main(int argc, char* argv[]) 
{
    double* A;
    double* B;
    double* X;
    int n;
    int thread_count;
    double start, finish;

    Get_args(argc, argv, &thread_count, &n);

    A = malloc(n*n*sizeof(double));
    B = malloc(n*sizeof(double));
    X = malloc(n*sizeof(double));

    Gen_matrix(A, n);
    Gen_vector(B, n);
    printf("\n\n");
    Print_matrix("The matrix 'A' is:", A, n);
    printf("\n\n");
    Print_vector("The vector 'B' is:", B, n);


    start = omp_get_wtime();
    Gauss_col(A, B, X, n, thread_count);    
    finish = omp_get_wtime();

    printf("\n\n");
    Print_vector("The solution is:", X, n);
    printf("\n\nElapsed time = %e seconds\n\n", finish - start);

    return 0;

}  /* main */

void Get_args(int argc, char* argv[], int* thread_count_p, int* n_p) {

   if (argc != 5) Usage(argv[0]);
   *thread_count_p = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[2], NULL, 10);
   if (*thread_count_p <= 0 || *n_p <= 0) Usage(argv[0]);

}  /* Get_args */

void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads> <matrix/array dimensions>\n", prog_name);
   exit(0);

}  /* Usage */

void Gen_matrix(double A[], int n) {
   
   int i, j;

   for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            A[i*n+j] = random()%10;
        }
    } 

}  /* Gen_matrix */

void Gen_vector(double x[], int n) {
   
   int i;

   for (i = 0; i < n; i++)
      x[i] = random()%10;

}  /* Gen_vector */

void Print_matrix(char* title, double y[], int n) {
   
   int   i,j;

   printf("%s\n", title);
   for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            printf("%10.3f ", y[i*n+j]);
        }
        printf("\n");   
   }

}  /* Print_vector */

void Print_vector(char* title, double y[], double n) {
   
   int   i;

   printf("%s\n", title);
   for (i = 0; i < n; i++)
      printf("%10.3f ", y[i]);
   printf("\n");

}  /* Print_vector */

void Gauss_col (double A[], double B[], double X[], int n, int thread_count) {
    
    int row, col;
    float multiplier;
    int norm;

// Gaussian elimination which transforms the matrix 'A' into an 'upper triangular' form
    for (norm = 0; norm < n - 1; norm++) {
#       pragma omp parallel for num_threads(thread_count) shared(A, B) private(multiplier,row,col)
        for (row = norm + 1; row < n; row++) {
            multiplier = A[row*n+norm] / A[norm*n+norm];
            for (col = norm; col < n; col++) {
	            A[row*n+col] -= A[norm*n+col] * multiplier;
            }
            B[row] -= B[norm] * multiplier;
        }
    }

    printf("\n\n");
    Print_matrix("After gaussian elimination:", A, n);

// Backward substitution using 'column oriented' algorithm
    for(row=0; row<n; row++){
        X[row] = B[row];
    }    

    for(col=n-1; col>=0; col--){
        X[col] /= A[col*n+col];
#       pragma omp parallel for num_threads(thread_count)  \
            schedule(runtime)
        for(row=0; row<col; row++){
            X[row] -= A[row*n+col] * X[col];
        }
    }

}  /* Gauss */