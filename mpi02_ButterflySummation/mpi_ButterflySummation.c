/****************************************************************************
* mpi_ButterflySummation.c
* The program will calculate global sum as butterfly algorithm.
* In this version, comm_sz has to be power of two to ensure function work correctly
*
* Trong Minh Vo, 16 Nov 2017
*
****************************************************************************/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

/* 
    Random function is seeded with system time with micro second 
    to diversify value obtain at each node at the same time
*/
int rand0_99(){
    long num;
    struct timeval tm;
    
    gettimeofday(&tm, NULL);
    srandom(tm.tv_sec + tm.tv_usec * 1000000ul);
    num = random();
    return num % 100 ;
}
/* 
    Function to check if input x is power of 2 or not
    x should be different than 0
    Return 0 if power of 2
*/
int CheckPowerOfTwo(int x)
{
    return (x & (x - 1));
}

int main(int argc, char *argv[]){
    int rank, numtasks, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int level, partner, sum, tsum, logLevel,sum_confrm, i;
    int send_buffer[2], recv_buffer[2];
    int *sum_arr = NULL;
     
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    
    /* 
    CheckPowerOfTwo(numtasks) shall return value != 0 
    if number of tasks is not power of two. Exit program
    */
    if(CheckPowerOfTwo(numtasks) != 0) { 
        printf("ERROR: Must be an power of 2 number of tasks!\n");
        MPI_Finalize();
        exit(0);
    }
    sum = rand0_99();
    printf("Init Value of each node: %d\n", sum);
   
    /* 
    Collect data from send_buffer with 1 by from other nodes with MPI_Allgather
    sum_confrm is calculated for later comparing in other nodes
    */
    send_buffer[0] = sum;
    sum_arr = (int *)malloc(sizeof(int) * numtasks);
    assert(sum_arr != NULL);
    MPI_Allgather(&send_buffer, 1, MPI_INT, sum_arr, 1, MPI_INT, MPI_COMM_WORLD);
    sum_confrm = 0;
    for (i=0; i<numtasks; i++) {
        sum_confrm +=sum_arr[i];
    }        
    /* 
    Butterfly sumation calculation
    partner node is calculated by inverse bit "level" with operator XOR in binary repersentation of "rank"
    */
    for (level=0; level<=((log(numtasks)/log(2))-1); level++) {
        logLevel = (int) (pow(2,level));
        //printf("At level:%d, loglevel: %d\n", level, logLevel);
        partner = rank^logLevel;
        //printf("At level:%d, rank: %d has partner: %d\n", level, rank, partner);
        
        send_buffer[0] = sum;
        /* Send current sum to partner node */
        MPI_Send(&send_buffer, 1, MPI_INT, partner, 1, MPI_COMM_WORLD);
        
        /* Receive sum from partner node */
        MPI_Recv(&recv_buffer, 1, MPI_INT, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        tsum = recv_buffer[0];
        
        /* Sum is added collectively and later distribute to other nodes */
        sum = sum + tsum;
        
    }
    /* Result is printing in all nodes to ensure that the algorithm run correctly every places */
    if (sum==sum_confrm){
         printf("Global sum is verified at rank: %d and has value: %d\n", rank, sum);
    } else {
        printf("ERROR: Global sum at butterfly is different with value calculated from MPI_Allgather!\n");
        MPI_Finalize();
        exit(0);
    }
    
    free(sum_arr);
    MPI_Finalize();
}
