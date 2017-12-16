/****************************************************************************
* mpi_bandwidth
*
* measure bandwidth and latency of MPI point-to-point communication
*
* Trong Minh Vo, 09 Nov 2017
*
* This code is based on the mpi_bandwidth.c code of Blaise Barney from 2005
****************************************************************************/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MAXTASKS      8192
/* Change the next three parameters to suit your case */
const int STARTSIZE = 1;
const int ENDSIZE = 2<<20;
const int ROUNDTRIPS = 100;
const double MEGA = 1024.0 * 1024.0; /* for converting bytes to MB */
const int MAX_PROCESSOR_NAME = 100;


int main (int argc, char *argv[]){

  int     numtasks, rank, n, i, j, nbytes, lenName;

  double duration, startTime, endTime, resolution;
  double currentBandWidth, maxBandWidth, minBandWidth, sumBandWidth, avgBandWidth;
  double maxOverall, avgOverall, minOverall;
  double arrBandWidth[MAXTASKS/2][3], tmpBandWidth[3];

  char    host[MAX_PROCESSOR_NAME], hostmap[MAXTASKS][MAX_PROCESSOR_NAME];
  char* buffer;

  /* =>=>=>=>>=>=>=> YOUR CODE HERE <=<=<=<=<=<=<=<=<=<=<= */

  /* Setup MPI, determine numtasks and rank. Abort if number of ranks is not even */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if (numtasks % 2 != 0) {
    printf("ERROR: Must be an even number of tasks!\n");
    MPI_Finalize();
    exit(0);
  }


  /* =>=>=>=>>=>=>=> YOUR CODE HERE <=<=<=<=<=<=<=<=<=<=<= */

  /************* Determine assignment of processes to hosts ************** */
  /* Each task determines its hostname and sends it to the master task     */
  /* Determine partner for each task                                       */
  /* Let master task print hostnames and partner ranks                     */

  /*all nodes send there core information to node 0 */
  MPI_Get_processor_name(host, &lenName);
  MPI_Send(&host, lenName, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
  //printf("Node %d was allocated in node %s\n",rank, host);

  /*initialitze buffer with maxsize possible for transmision and recieving data*/
  /*all of layout is initialize with char 0 */
  buffer = (char*) calloc (ENDSIZE, sizeof(char));
    
  /*if rank is even number, rank shall send package*/
  if (rank%2 == 0) {
    
    if(rank == 0) {
      resolution = MPI_Wtick();
      for (j=0; j<numtasks; j++) {
        MPI_Recv(&hostmap[j], lenName, MPI_CHAR, j, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
      printf("\n******************** MPI Bandwidth Test ********************\n");
      printf("Message starts with size of = %d bytes\n", STARTSIZE);
      printf("Message finish with size of size= %d bytes\n", ENDSIZE);
      printf("Incremented by power of 2 of number of previous bytes each iteration\n");
      printf("Roundtrips per iteration= %d\n", ROUNDTRIPS);
      printf("MPI_Wtick resolution = %e\n", resolution);
      printf("************************************************************\n");
      for (i=0; i<numtasks; i+=2)
        if(rank%2 == 0){ 
          printf("Task %d on %s \t is partner of task %d on %s\n", i, hostmap[i], i+1, hostmap[i+1]);
        }
      printf("************************************************************\n");
      if(STARTSIZE != 1) {
        printf("\n\nATTENTION: Number of STARTSIZE is not starting with 1!!!\n");
        printf("Transmision shall start with the bigger package with size of power of 2\n");
      }
    }
    /* =>=>=>=>>=>=>=> YOUR CODE HERE <=<=<=<=<=<=<=<=<=<=<= */
    /*************************** First half of tasks *******************************/
    /* Each rank sends a message of size n to partner task, then receives message  */
    /* from partner. Take timing of communication and repeat for ROUNDTRIPS times */
    /* Compute bandwidth statistics considering message size and arrBandWidth.         */
    for (n=1; n<=ENDSIZE; n=n<<1) {
      if (n>STARTSIZE) {
        minBandWidth = 1E+99;
        avgBandWidth = 0.0;
        maxBandWidth = 0.0;
        sumBandWidth = 0.0;
        for (i=1; i<=ROUNDTRIPS; i++){
          startTime = MPI_Wtime();
          MPI_Send(buffer, n, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD);
          MPI_Recv(buffer, n, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          endTime = MPI_Wtime();
          duration = endTime - startTime;
          resolution = MPI_Wtick();
            
          nbytes =  sizeof(char) * n;  
          currentBandWidth = ((double)nbytes * 2) / duration;
          
          if (currentBandWidth > maxBandWidth ) {
            maxBandWidth = currentBandWidth;
          }
          if (currentBandWidth < minBandWidth ) {
            minBandWidth = currentBandWidth;
          }
          sumBandWidth = sumBandWidth + currentBandWidth;
        }

        minBandWidth = minBandWidth/(MEGA);
        avgBandWidth = (sumBandWidth/ROUNDTRIPS)/(MEGA);
        maxBandWidth = maxBandWidth/(MEGA);
        if(rank == 0) {
          /* Keep track of my own arrBandWidth first */
          arrBandWidth[0][0] = minBandWidth;  
          arrBandWidth[0][1] = avgBandWidth;    
          arrBandWidth[0][2] = maxBandWidth;

          /* Initialize overall averages */
          minOverall = 0.0;
          avgOverall = 0.0;
          maxOverall = 0.0;

          for (j=2; j<numtasks; j=j+2) {
            MPI_Recv(&arrBandWidth[j], 3, MPI_DOUBLE, j, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          }
          printf("\n");
          printf("** Message size:  %8d  *** MIN \t/ AVG   \t/ MAX (MB/sec)\n",n);
            for (j=0; j<numtasks; j+=2) { 
              printf("   task pair: %4d - %4d:    %6.2f \t/ %6.2f \t/ %6.2f \n", j, j+1, arrBandWidth[j][0], arrBandWidth[j][1], arrBandWidth[j][2]);
              minOverall = minOverall + arrBandWidth[j][0];
              avgOverall = avgOverall + arrBandWidth[j][1];
              maxOverall = maxOverall + arrBandWidth[j][2];
            }
          printf("   OVERALL AVERAGES:          %6.2f \t/ %6.2f \t/ %6.2f \n\n", minOverall/(numtasks/2), avgOverall/(numtasks/2), maxOverall/(numtasks/2));
        } else {
          tmpBandWidth[0] = minBandWidth; 
          tmpBandWidth[1] = avgBandWidth;    
          tmpBandWidth[2] = maxBandWidth; 
          MPI_Send(tmpBandWidth, 3, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        }
      }
    }
  } else { 
    /* =>=>=>=>>=>=>=> YOUR CODE HERE <=<=<=<=<=<=<=<=<=<=<= */
    /**************************** second half of tasks ***************************/
    /* These tasks do nothing more than send and receive with their partner task */
    /* if rank is odd number, rank shall recive package and send back to next previous rank*/
    for (n=1; n<=ENDSIZE; n=n<<1) {
      if (n>STARTSIZE) {
        for (i=1; i<=ROUNDTRIPS; i++){
          MPI_Recv(buffer, n, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          MPI_Send(buffer, n, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD);
        }
      }
    }
  }

  free(buffer);
  MPI_Finalize();
}
