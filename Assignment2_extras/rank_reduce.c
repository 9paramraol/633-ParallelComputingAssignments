

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "mpi.h"

int nodeToGroupMap[93] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

// takes the starting of the hostname string and returns the groups to which the nodes belongs to.
int getGroupNumberFromHostname(char* hostname){
  int csewsNumber = atoi(hostname+5);
  return nodeToGroupMap[csewsNumber];
}

int getMacNum(char* hostname){
	int csewsNumber = atoi(hostname + 5);
	return csewsNumber;
}

int main (int argc, char *argv[]){
	// will be set after wards
	int root = 1;
	char hostname[MPI_MAX_PROCESSOR_NAME];

  int D = 2048*1024/sizeof(double);
  double *buff = (double*)malloc(sizeof(double)*D);
  double *buff_sum = (double*)malloc(sizeof(double)*D);
  for(int i=0;i < D; ++i){
      buff[i] = 1.0;
      buff_sum[i] = 0.0;
  }
	// start

	MPI_Init ( &argc, &argv);

  
  // CHANGE IT AFTERWARDS NOW JUST FOR TESTING
  



    double sTime = MPI_Wtime();

	// getting the numproc and self rank
  	int numProc, rank, processorNameLen;
  	MPI_Comm_size (MPI_COMM_WORLD, &numProc);
  	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	// initializing the buffer for all the processes
	// get the name of the processor
    MPI_Get_processor_name (hostname, &processorNameLen);
    int machineNum = getMacNum(hostname);
    int color = getGroupNumberFromHostname(hostname)*100 + machineNum;

    if(rank==root)
      rank = (-1)*rank;
    // this type of tag ensure that the given root will always be the 0th indexed element
    MPI_Comm newComm;
    MPI_Comm_split (MPI_COMM_WORLD, color, rank, &newComm);
    
  	MPI_Reduce(buff, buff_sum, D, MPI_DOUBLE, MPI_SUM, 0, newComm);
    
    double eTime = MPI_Wtime() - sTime;
    double maxTime;
    MPI_Reduce(&eTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(!rank)
      printf("%lf\n", maxTime);
  	// if(rank < 0)
  	// 	for(int i=0;i < D; ++i) 
  	// 		printf("%lf ,",buff_sum[i]);

    MPI_Finalize();
}