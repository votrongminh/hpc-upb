/****************************************************************************
* mpi_pi_estimation
*
* 
*
* Trong Minh Vo, 16 Nov 2017
*
* 
****************************************************************************/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


double rand01()
{
    struct timeval tm;
    
    gettimeofday(&tm, NULL);
    srand(tm.tv_usec * tm.tv_sec);
    return (double)rand() / (double)RAND_MAX  ;
}
double rand01_()
{
    return (double)rand() / (double)RAND_MAX  ;
}

int main (int argc, char *argv[]){
  int     numtasks, rank;
  long long int totalToss, rankToss, numberInCircle, toss, sumNumberInCircle;
  double x, y, distanceSquared, piEstimate;
  long long int buffer[2], sendbuf[2];
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  
  if(rank == 0) {
    totalToss = atoi(argv[1]);
    printf("Provide total of Toss: %lld\n", totalToss);
    
    buffer[0] = totalToss/numtasks; //distribute tasks to all nodes
    MPI_Bcast( &buffer, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

  
    //calculation of sum in node 0
    rankToss = buffer[0];
    printf("Value of rankToss received by rank %d: %lld\n",rank, rankToss);
    numberInCircle = 0;
      for(toss=0; toss<rankToss; toss++){
        x = rand01();
        y = rand01();
        //printf("Radom value generated: %4.4f  %4.4f\n",x, y);
        distanceSquared = x*x + y*y;
        if (distanceSquared <= 1) {
          numberInCircle++;
        }
      }
      printf("Value of numberInCircle calculated by rank %d: %lld\n",rank, numberInCircle);
      sendbuf[0] = numberInCircle;
  
    sumNumberInCircle = 0;
    buffer[0] = sumNumberInCircle;
  
    MPI_Reduce(&sendbuf, &buffer, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    sumNumberInCircle = buffer[0];
    printf("\nTotal of sumNumberInCircle: %lld\n", sumNumberInCircle);
    
    piEstimate = 4*sumNumberInCircle/((double) (totalToss));
    printf("\n************* Result of PI ESTIMATION *************\n");
    printf("\n                    %4.8f\n", piEstimate);
    printf("\n***************************************************\n\n");
    } else {
    MPI_Bcast( &buffer, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    rankToss = buffer[0];
    printf("Value of rankToss received by rank %d: %lld\n",rank, rankToss);
    
    numberInCircle = 0;
      for(toss=0; toss<rankToss; toss++){
        x = rand01();
        y = rand01();
        distanceSquared = x*x + y*y;
        if (distanceSquared <= 1) {
          numberInCircle++;
        }
      }
      
      printf("Value of numberInCircle calculated by rank %d: %lld\n",rank, numberInCircle);
      sendbuf[0] = numberInCircle;
      MPI_Reduce(&sendbuf, &buffer, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  }
  MPI_Finalize();
}

