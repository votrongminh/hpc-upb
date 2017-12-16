# Exercise: mpi_ButterflySummation

The exercise has two parts. 
The first part deal with MPI_Allgather to collect summands from all nodes.
Then sum value is calculated manual at each node. This value shall be later used
to verify if summation from butterfly algorithm works

The second part we deal with butterfly multiplication where number of nodes
are power of two.
Partner node is calculated by inverse bit "level" with operator XOR in binary representation of "rank"
level is defined as denote power of two of total rank. eg: rank=4 then level=2, rank=8 then level=3
The program iterates for each level and perform:
 - send current sum to its partner node
 - receive current sum from its partner node
 - update new current sum value from received value
 
Input of the program is generated randomly at each node via rand0_99
Random function is seeded with system time with micro second 
to diversify value obtain at each node at the same time

If at the end, value of butterfly summation is equal with MPI_Allgather summation calculation.
The report shall be printed for all node to prove that all nodes had calculated correctly.
Else error message shall be printed

File and folder structure
mpi02_ButterflySummation            #folder name
|-->    Makefile                    #makefile
|-->    mpi_ButterflySummation      #complied executable
|-->    mpi_ButterflySummation.c    #source code
|-->    README.md                   #readme file

### Compiling and running your program on OCuLUS ###

```shell
# login to frontend
ssh fe.pc2.uni-paderborn.de

# load compiler and openmp
module load openmpi
module load gcc

# compile program
mpicc -o mpi_ButterflySummation mpi_ButterflySummation.c -lm
or 
make all

# submit program to Oculus as an interactive job
ccsalloc -I --res=rset=8:ncpus=1 ompi -- ./mpi_ButterflySummation
```
