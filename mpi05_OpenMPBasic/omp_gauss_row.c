/* File:     
 *     omp_gauss_row.c 
 *
 *
 * Purpose:  
 *          Solves an upper triangular system when the matrice A[n][n] is provided,
 *          together with the "right-hand side" solutions into the array B[n]. 
 *          The solutions of the system will be recorded in the array X[n].
 *
 * Compile:  
 *    gcc -g -Wall -fopenmp -o omp_gauss_row omp_gauss_row.c 
 * Usage:
 *    ./omp_gauss_col <thread_count> <matrix dimensions> [<measure>]
 *    Eg.1: ./omp_gauss_row 4 10
 *    Eg.2: ./omp_gauss_row 4 10000 measure
 *
 * Note: measure flag shall be turned on when we want to measure performance different 
 * between different loop schedule scheme.
 * When measure flag is turned on. The programe shall loop through all schedule scheme
 * and pre-define chunk_sizes and thread_count
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

/* Serial functions */
void Get_args(int argc, char* argv[], int* thread_count_p, int* n_p, char* exec_mode);
void Usage(char* prog_name);
void Gen_matrix(double A[], int n);
void Gen_vector(double x[], int n);
void Print_matrix(char* title, double y[], int n);
void Print_vector(char* title, double y[], double n);
void Scheduling_Analysis(double A[], double B[], double X[], int n);

/* Parallel functions */
void Gauss_Uppertriangular (double A[], double B[], double X[], int n, int thread_count);
void Gauss_Row_BackwardSubstitution (double A[], double B[], double X[], int n, int thread_count);


#define NUM_ELEMS(x)  (sizeof(x) / sizeof((x)[0]))

int main(int argc, char* argv[]) 
{
    double* A;
    double* B;
    double* X;
    int n;
    int thread_count;
    double start, finish;
    char* exec_mode;
    
    exec_mode = malloc(20*sizeof(char));
    A = malloc(n*n*sizeof(double));
    B = malloc(n*sizeof(double));
    X = malloc(n*sizeof(double));
    
    Get_args(argc, argv, &thread_count, &n, exec_mode);
    
    Gen_matrix(A, n);
    Gen_vector(B, n);
    
	/* Only call Analysis sub routine if measure is turned on */
    if (strcmp(exec_mode,"measure") == 0) {
        Scheduling_Analysis(A, B, X, n);
        printf("\n");
        return 0;
    }
    
    if (n <=10) {
        printf("\n\n");
        Print_matrix("The matrix 'A' is:", A, n);
        printf("\n\n");
        Print_vector("The vector 'B' is:", B, n);
    } else {
        printf("Matrix size is too big...! \n");
        printf("Printing of Matrix is skipped. Only Matrix with size < 10 will get printed! \n");
    }
    
	/* Start calculation */
    start = omp_get_wtime();    
    Gauss_Uppertriangular(A, B, X, n, thread_count);  
    Gauss_Row_BackwardSubstitution(A, B, X, n, thread_count);    
    finish = omp_get_wtime();

    if (n <=10) {
        printf("\n\n");
        Print_vector("The solution is:", X, n);
        printf("\n\nElapsed time = %e seconds\n\n", finish - start);
    } else {
        printf("Matrix got solved successfully...! \n");
        printf("Printing result is skipped. Only Matrix with size < 10 will get printed! \n");
    }
    
    printf("\n\nElapsed time = %e seconds\n\n", finish - start);
        
    return 0;

}  /* main */

/* Sub routine used to measure performance different loop scheduling in OpenMP 
Sub routine only targets to backward substitution as requirement of 
Exercises 5.4.e (Backward Substitution) in Pacheco's book.*/
void Scheduling_Analysis(double A[], double B[], double X[], int n) {

    int chunk_sizes[] = {0, 2, 4, 8, 16, 32, 61, 128};
    int scheduling_types[] = {omp_sched_static, omp_sched_dynamic, omp_sched_guided, omp_sched_auto };
    int threads[] = {1, 2, 5, 10, 20};
    int i, j, k;
    double start, finish;
    
    for(i=0;i<NUM_ELEMS(scheduling_types);i++) {
		
		/* The scheduling kinds that are available are defined in omp.h as this enum below
			typedef enum omp_sched_t {
				omp_sched_static = 1,
				omp_sched_dynamic = 2,
				omp_sched_guided = 3,
				omp_sched_auto = 4
			} omp_sched_t;
		*/
        switch(scheduling_types[i]) {
            case 1:
                printf("\n\nSchedule Type: Static");
                printf("\nChunk Size: 0 means chunk size shall be set to default by OpenMP");
                break;
            case 2:
                printf("\n\nSchedule Type: Dynamic");
                printf("\nChunk Size: 0 means chunk size shall be set to default by OpenMP");
                break;
            case 3:
                printf("\n\nSchedule Type: Guided");
                printf("\nChunk Size: 0 means chunk size shall be set to default by OpenMP");
                break;
            case 4:
                printf("\n\nSchedule Type: Auto");
                printf("\nChunk Size is ignored in omp_sched_auto ");
                break;
            default:            
                printf("\n\nUnknown Schedule Type...");
                return;
        }
        
        printf("\nChunk Size:\t\t");
        for(k=0;k<NUM_ELEMS(chunk_sizes);k++) {
            printf("\t   %d", chunk_sizes[k]);
        }
        for(j=0;j<NUM_ELEMS(threads);j++) {
            printf("\nWith %d threads |\t", threads[j]);
            //printf("At chunk: \t\t ExecTime:  seconds\n");
            for(k=0;k<NUM_ELEMS(chunk_sizes);k++) {
                omp_set_schedule(scheduling_types[i], chunk_sizes[k]);
                start = omp_get_wtime();      
                Gauss_Row_BackwardSubstitution(A, B, X, n, threads[j]);    
                finish = omp_get_wtime();
                printf("\t : %4.2f" , finish - start);
            }
        }
    }

}  /* Scheduling_Analysis */

void Get_args(int argc, char* argv[], int* thread_count_p, int* n_p, char* exec_mode) {

   //if (argc != 5) Usage(argv[0]);
   *thread_count_p = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[2], NULL, 10);
   
   if (argc == 4) {
       strcpy(exec_mode, argv[3]);
       printf("Get_args: exec_mode: %s, argv[3]: %s\n", exec_mode, argv[3]);
   }
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
            printf("%10.1f ", y[i*n+j]);
        }
        printf("\n");   
   }

}  /* Print_vector */

void Print_vector(char* title, double y[], double n) {
   
   int   i;

   printf("%s\n", title);
   for (i = 0; i < n; i++)
      printf("%10.1f ", y[i]);
   printf("\n");

}  /* Print_vector */

/*  Gaussian elimination which transforms the matrix 'A' into an 'upper triangular' form */
void Gauss_Uppertriangular (double A[], double B[], double X[], int n, int thread_count) {
    
    int row, col;
    float multiplier;
    int norm;
	
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
    printf("matrix size: %d\n", n);
    if (n <= 10) {
        printf("\n\n");
        Print_matrix("After gaussian elimination:", A, n);
    }

}  /* Gauss_Uppertriangular */

/* Implement Backward Substitution of Gaussian Elimination algorithm with row oriented*/
void Gauss_Row_BackwardSubstitution (double A[], double B[], double X[], int n, int thread_count) {
    
    int row, col;
    double tmp_x;
    // Backward substitution using 'row oriented' algorithm
    for (row = n-1; row >= 0; row--) {
        tmp_x = B[row];
#       pragma omp parallel for num_threads(thread_count)  \
            schedule(runtime) \
            reduction(-:tmp_x)
        for (col = row+1; col < n; col++){
            tmp_x -= A[row*n+col]*X[col];;
        }
        X[row] = tmp_x;
        X[row] /= A[row*n+row];
    }
} /* Gauss_Row_BackwardSubstitution */
