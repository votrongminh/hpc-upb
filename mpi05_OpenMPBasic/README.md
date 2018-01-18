# Exercise: OpenMP Basics #

In these exercises you will implement a parallel solver for linear equations systems. The problem description given in the Programming Exercises 5.4 (Backward Substitution) and 5.5 (Gaussian Elimination) in Pacheco's book.

# Delivery Package#
Zip file: mpi05_OpenMPBasic.zip
  |_ README.md          --> Solution ideas and running steps
  |_ omp_gauss_row.c    --> Gaussian Elimination with backward substitution implemented with OpenMP
  |_ omp_gauss_col.c    --> Gaussian Elimination with backward substitution implemented with OpenMP
  


## Tasks ##

###Exercise 5.4 in Pacheco Solutions ###

a.) Determine whether the outer loop of the row-oriented algorithm can be parallelized.
    # Answer: 
        The outer loop of the row-oriented algorithm cannot be parallelized.
    
    # Reason: 
        Value of X is dependant to value of previous calculation 
        For example X[n-2] is depending on value of X[n-1] in equation "x[row] -= A[row][col]*x[col];"
        where row = n-2 and col = n-1

b.) Determine whether the inner loop of the row-oriented algorithm can be parallelized.
    # Answer: 
        The inner loop of the row-oriented algorithm can be parallelized.
    # Reason: 
        There is no data dependency between loops. However, all loop need to access to same critical memory location x[row]
        Hence, x[row] is now tracked by tmp_x variable and update value with reduction subtract operator when join threads

            for (col = row+1; col < n; col++)
                x[row] -= A[row][col]*x[col];

c.) Determine whether the (second) outer loop of the column-oriented algorithm can be parallelized.
    Answer:  the (second) outer loop of the column-oriented algorithm can not be parallelized.
    Reason: 
    Let consider when col = n-1 and col = n-2
    At col = n-2, Value of X[0] to X[n-3] are dependant to its value at iteration of col = n-1


d.)Determine whether the inner loop of the column-oriented algorithm can be parallelized.
    Answer: 
        The inner loop of the column-oriented algorithm can be parallelized.
    Reason: 
        There is no dependency in calculation of x[row] in every loop of row
        
        
e.) Write one OpenMP program for each of the loops that you determined could be parallelized.
    Answer: The two loop of backward substitution are implemented in Gauss_Row_BackwardSubstitution and Gauss_Col_BackwardSubstitution

    
f.) Modify your parallel loop with a schedule(runtime) clause and test the program with various schedules. If your upper triangular system has 10,000 variables, which schedule gives the best performance?
    Answer: 
        The performance of upper triangular system is dependant on number of calculation variable, thread counts, scheduling types and chunk size of scheduling algorithm
        The detail of performance variation can be seen when "measure" flag is turned on E.g./omp_gauss_col 4 10000 measure
        We can see dynamic scheduling always carry overhead calculation when chunk size is small compare to static and guided.
        When chunk increase dynamic schedule gain the efficiency but still not that good compare to static one
        At execution with 10 threads, execution time is considered minimum and align with auto scheduling which showed to be best in most of the combination
        So in order to tailor to best performance, we should aware of input factors and optimize with static scheme else we should leave to default compiler to decide


###Exercise 5.5 in Pacheco Solutions ###
    The solutions are implemented in omp_gauss_col.c omp_gauss_row.c
    Execution can be done
    Eg.
        ./omp_gauss_col 4 10 
        ./omp_gauss_row 4 10


Compile via
```shell
gcc -g -Wall -fopenmp -o omp_gauss_col omp_gauss_col.c 
gcc -g -Wall -fopenmp -o omp_gauss_row omp_gauss_row.c 
````

Run via
```shell
        ./omp_gauss_col 4 10 
        ./omp_gauss_row 4 10

For performance measuring
        ./omp_gauss_col 4 10000 measure
        ./omp_gauss_row 4 10000 measure
        
    Thread counts has no affect in measuring mode
        