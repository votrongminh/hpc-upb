#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{

  int   com_sz, rank, len;
  char hostname[MPI_MAX_PROCESSOR_NAME];
  const int MAX_MESSAGE_LENGTH = MPI_MAX_PROCESSOR_NAME+100;
  char buffer[MAX_MESSAGE_LENGTH];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &com_sz);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  MPI_Get_processor_name(hostname, &len);

  if (rank != 0) {
    snprintf(buffer, MAX_MESSAGE_LENGTH, "Hello from rank %d on host %s!\n", rank, hostname);
    MPI_Send(buffer,strlen(buffer)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  } else {
    printf("MASTER (running on host %s): Number of MPI tasks is: %d\n",hostname,com_sz);
    for(int i=1; i<com_sz; i++) {
        MPI_Recv(buffer, MAX_MESSAGE_LENGTH, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%s",buffer);
    }
  }

  MPI_Finalize();

}
