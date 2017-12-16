#Delivery Package#
Zip file: mpi_bandwidth_minhvo.zip
  |_ mpi_bandwidth.c --> Solution of the exercise
  |_ Makefile        --> Makefile
  |_ README.md       --> Solution ideas and running steps


# Exercise: Solution ideas #
## Send and receive pair strategy##
    The even nodes (eg. node 0, 2, 4,...) shall responsible as send initiator to send package to the odd nodes (eg. 1, 3, 5,...) which play as response nodes
    The pair shall be determined by two adjacent nodes (eg. 0-2, 1-3,...)
    Once odd nodes receive package, immediately send the package back to even nodes in its pair

    Even nodes shall be responsible for bandwidth calculation then send all required data encapsulated in array tmpBandWidth[] with has layout of {minBandWidth, avgBandWidth, maxBandWidth} to node 0.
    Node 0 then print all of required message to system console

    Node 0 has to display also node name information in which task is running on and its partner.
    Hence all node has to communicate its host-name to node 0

## Node communications ##
    All nodes once complete calculation of bandwidth must send its calculated bandwidth

## Message buffer initialization ##
    Buffer shall be initialized with max size defined in ENDSIZE. All cell is initialized with 0 via calloc function.
    Size of buffer for each transmission is defined in argument COUNT of function MPI_Send or MPI_Receive.
## Algorithm ##
    For each send initiators, we need to send series of package with size shall be varied each time with power of 2. Eg. 1, 2, 4, 8,.... Package start with size n>STARTSIZE bytes and end by n<=ENDSIZE.
    For each package, we repeat send/receive process ROUNDTRIPS times.
    Min bandwidth is initialized with 1E+99 and Max bandwidth is initialized with 0. For each iteration of ROUNDTRIPS, min and max bandwidth shall be updated for each pair. Average bandwidth is calculated from average of all bandwidth per ROUNDTRIPS.
    Bandwidth of transmission is seen as total of bytes transmitted and received back (2 x package size) per total transmission time

    For each response node, we receive from its pair and immediately send back the same data with same size

# Exercise: Running steps #


unzip solution package to local folder

```shell
# login to frontend
ssh fe.pc2.uni-paderborn.de

# load compiler and openmp
module load openmpi
module load gcc

#change directory to mpi_bandwidth_minhvo
cd mpi_bandwidth_minhvo

# compile program
mpicc -o mpi_bandwidth mpi_bandwidth.c

# submit program to Oculus as an interactive job
ccsalloc -I --res=rset=8:ncpus=1 ompi -- ./mpi_bandwidth
```