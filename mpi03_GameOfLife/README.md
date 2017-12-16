# Exercise: MPI Conway Game of Life #
# Delivery Package#
Zip file: mpi_bandwidth_minhvo.zip
  |_ GameOfLife_Task2_Down.c    --> Task 2 solution for ExchangeGhostlines with blocks falling down
  |_ GameOfLife_Task2_Up.c      --> Task 2 solution for ExchangeGhostlines with blocks falling up
  |_ GameOfLife_Task2_Ssend.c   --> Task 2 solution for ExchangeGhostlines with Ssend verification
  |_ GameOfLife_Task3.c         --> Task 3 solution for CalculateUpdate as GameOfLife rules
  |_ GameOfLife_Task4.c         --> Task 4 solution for ExchangeGhostlinesNonblocking
  |_ Makefile                   --> Makefile
  |_ README.md                  --> Solution ideas and running steps

  
# Exercise: Solution Quick Report #

  
### Task 1 ###
Compilation and run with no issue
Note: RLE_Parsing.c got small change to adapt name of output file in numberic order
eg. ConwayOutput_Step00.ppm

### Task 2 ###
Implement body of ExchangeGhostlines to exchange ghostline between ranks
Order of sending and recieving of top line is keeps as
Rank0 send its ghostline to Rank1. 
Rank1 receives ghostline from Rank0 then send its ghostline to Rule2
The process continue till rank end
Since Rank0 top ghostline at very top of matrix, its recieved ghostline has to be initialize as 0

A reversed process is apply for ghostline bottom
MPI_Ssend is also applied to verify if deadlock happens


### Task 3 ###
Implement body of CalculateUpdate function for GameOfLife rules
	Rules implemented:
		0: A living cell is represented as a 1, a dead cell is represented as a 0
		1: A living cell stays alive if its surrounded by 2 or 3 living cells
		2: A dead cell comes alive if it's surrounded by exactly 3 live cells
		3: In all other cases the cell becaomes dead by over/underpopulation
Cell neighbor calculation:
For each normal cells, we have to consider 8 cells sourounding it.
A counter variable is created to keep track of its alive neighbor then later apply GameOfLife rules
For cells at column at most left [0] and right [sizeX-1], 
Perform special rule is applied due to lacking of neighbor column
For top row and bottom row, we already have ghostlines recieved from other node. Hence we treat these cells as normal cell


### Task 4 ###
Implement body of ExchangeGhostlinesNonblocking to exchange ghostline between ranks
MPI_Wait is used to wait until request used in MPI_Isend/ MPI_Irecv complete


### Task 5 ###
Run puffer2.rle for 100 steps, getting images every 5. How does the pattern behave?
Answer: pattern behave normally as we do not change logic of GameOfLife.
Only number of ppm files are generated lesser every 5 steps for 1 ppm file



# Exercise: Running steps #

Add modules
```shell
module add intel-mpi
module add intel/compiler/15.0.1
```


### Task 2 ###
Compile via
```shell
mpiicc -g -Wall -std=c99 -o GameOfLife GameOfLife_Task2_Down.c RLE_Parsing.c
or 
mpiicc -g -Wall -std=c99 -o GameOfLife GameOfLife_Task2_Up.c RLE_Parsing.c
or 
mpiicc -g -Wall -std=c99 -o GameOfLife GameOfLife_Task2_Ssend.c RLE_Parsing.c
```
Run via
```shell
ccsalloc -I --res=rset=1:ncpus=8:mpiprocs=8 impi -- ./GameOfLife TestFile.rle 10 1
````
Transfer the created images back to your machine and view them.



### Task 3 ###

Compile via
```shell
mpiicc -g -Wall -std=c99 -o GameOfLife GameOfLife_Task3.c RLE_Parsing.c

```
Run via
```shell
ccsalloc -I --res=rset=1:ncpus=8:mpiprocs=8 impi -- ./GameOfLife TestFile.rle 100 1
or 
ccsalloc -I --res=rset=1:ncpus=8:mpiprocs=8 impi -- ./GameOfLife puffer2.rle 100 1
or 
ccsalloc -I --res=rset=1:ncpus=8:mpiprocs=8 impi -- ./GameOfLife 17columnheavyweightvolcano.rle 100 1
````
Transfer the created images back to your machine and view them.



### Task 4 ###
Compile via
```shell
mpiicc -g -Wall -std=c99 -o GameOfLife GameOfLife_Task4.c RLE_Parsing.c

```
Run via
```shell
ccsalloc -I --res=rset=1:ncpus=8:mpiprocs=8 impi -- ./GameOfLife TestFile.rle 100 1



### Task 5 ###
Compile via
```shell
mpiicc -g -Wall -std=c99 -o GameOfLife GameOfLife_Task4.c RLE_Parsing.c

```
Run via
```shell
ccsalloc -I --res=rset=1:ncpus=8:mpiprocs=8 impi -- ./GameOfLife puffer2.rle 100 5