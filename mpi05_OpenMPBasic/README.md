# Exercise: OpenMP Basics #

In this exercises you will implement a parallel solver for linear equations systems. The problem description given in the Programming Exercises 5.4 (Backward Substitution) and 5.5 (Gaussian Elimination) in Pacheco's book.

## Tasks ##

### Backward Substitution ###

Tasks see Exercise 5.4 in Pacheco.
a.) Determine whether the outer loop of the row-oriented algorithm can be parallelized.
Answer: The outer loop of the row-oriented algorithm cannot be parallelized.
Reason: Value of X is dependant to value of previous calculation 
For example X[n-2] is depending on value of X[n-1] in equation "x[row] -= A[row][col]*x[col];"
where row = n-2 and col = n-1
b.) Determine whether the inner loop of the row-oriented algorithm can be parallelized.
Answer: The inner loop of the row-oriented algorithm can be parallelized.
Reason: There is no data depenency between loops. However all loop need to access to same critical memory location x[row]
Hence, x[row] is now tracked by tmp_x variable and update value with reduction substract operator when join threads

	for (col = row+1; col < n; col++)
		x[row] -= A[row][col]*x[col];

c.) Determine whether the (second) outer loop of the column-oriented algorithm can be parallelized.
Answer:  the (second) outer loop of the column-oriented algorithm can not be parallelized.
Reason: 
Let consider when col = n-1 and col = n-2
At col = n-2, Value of X[0] to X[n-3] are dependant to its value at interation of col = n-1


d.)Determine whether the inner loop of the column-oriented algorithm can be parallelized.
Answer: The inner loop of the column-oriented algorithm can be parallelized.
Reason: There is no dependancy in calculation of x[row] in every loop of row
e.)

### Gaussian Elimination ###

Tasks see Exercise 5.5 in Pacheco.

## Grading ##

A correct solution of this exercise will be awarded with 10 points.
