# Exercise 04: Pthreads Basics #
# Delivery Package#
Zip file: pthread_pi_estimation.zip
  |_ pthread_pi_estimation_increment.c   --> Task 2 solution for ExchangeGhostlines with blocks falling down
  |_ README.md                           --> Solution ideas and running steps
  |_ ExecutionGraph.png                  --> Sample of ExecTime Graph Output
  |_ ExecTime.dat                        --> Sample of ExecTime Data Output

  
# Exercise: Solution Quick Report #

  The program recieve two arguments as number of threads and total of tosses as inputs
  We shall fix total of tosses and varies number of threads in a for loop
  The execution time is stored in array structure with size of number of loops through thread count
  We shall use these data for calculation of execution time, speed up and efficicency
  
  SpeedUp = Tserial/Tparalell
  Efficiency = Tserial / (numberOfThread * Tparalell)
    
 The program shall output a data file "ExecTime.dat" which is used to feed to GNUPLOT for ploting purpose
 With the provided GNUPLOT command, user can plot themsefves for Execution Time relation hence understand of SpeedUp and Efficiency factors
 The paralell execution did not show much adavan

# Exercise: Running steps #


Compile via
```shell
gcc -g -Wall -o pthread_pi_estimation_increment pthread_pi_estimation_increment.c -lm -lpthread

````

Run via
```shell

ccsalloc -I -n 1 -t 10m
./pthread_pi_estimation_increment 10 1000000

````
exit to Oculus normal mode
Plot graph via:

gnuplot -e "\
set terminal png size 800,600; \
set autoscale y; \
set title 'Execution Time Parallel vs Serial';\
set xlabel 'Thread Counts';\
set ylabel 'Time (second)';\
set output 'ExecutionGraph.png'; \
plot 'ExecTime.dat' u (column(0)):2:xtic(1)  w l title 'Time_SerialExec',\
     'ExecTime.dat' u (column(0)):3:xtic(1) w l title 'Time_ParallelEach', \
     'ExecTime.dat' u (column(0)):4:xtic(1) w l title 'Time_ParallelEnd'"
     