

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
    // this type of tag ensure that the given root will always be the 0th indexed element
    if(rank == root)
    	rank = (-1)*rank;

  	// nodes' communicator
  	int ncolor = machineNum;
  	int nodeGroupRank, nodeGroupSize;
  	MPI_Comm nodeGroupComm;

  	MPI_Comm_split (MPI_COMM_WORLD, ncolor, rank, &nodeGroupComm);
 	  MPI_Comm_rank( nodeGroupComm, &nodeGroupRank );
  	MPI_Comm_size( nodeGroupComm, &nodeGroupSize );
	
	  // Group's communicator ------------------------------------------------
  	int intraGroupRank, intraGroupSize;
  	MPI_Comm intraGroupComm;  	
  	intraGroupRank = -1; intraGroupSize = -1;
  	if( nodeGroupRank == 0){
  		int color = getGroupNumberFromHostname(hostname) + 100;	
		MPI_Comm_split (MPI_COMM_WORLD, color, rank, &intraGroupComm);
  	}
  	else
  		MPI_Comm_split (MPI_COMM_WORLD, MPI_UNDEFINED, rank, &intraGroupComm);

  	if(nodeGroupRank == 0){
	 	MPI_Comm_rank( intraGroupComm, &intraGroupRank );
  		MPI_Comm_size( intraGroupComm, &intraGroupSize );
  	}

  	// group's leader subgroup for the communication -------------------------
  	int interGroupSize; interGroupSize = -1;
  	MPI_Comm interGroupLeaderComm;
  	if(intraGroupRank == 0)
  	  	MPI_Comm_split (MPI_COMM_WORLD, 1000, rank, &interGroupLeaderComm);
  	else
    	MPI_Comm_split (MPI_COMM_WORLD, MPI_UNDEFINED, rank, &interGroupLeaderComm);

    if(intraGroupRank == 0)
    	MPI_Comm_size ( interGroupLeaderComm, &interGroupSize);
  	

  	// reduction at node level -------------------------------------------------
  	if( nodeGroupSize > 1){
      MPI_Reduce(buff, buff_sum, D, MPI_DOUBLE, MPI_SUM, 0, nodeGroupComm);
  		// printf("%d-%d-%d\n",ncolor,rank,nodeGroupRank);
      double *temp = buff;
      buff = buff_sum;
      buff_sum = temp;
  	}
  	// reduction at group level
  	if( nodeGroupRank == 0 && intraGroupSize > 1 ){
  		MPI_Reduce(buff, buff_sum, D, MPI_DOUBLE, MPI_SUM, 0, intraGroupComm);
  		double *temp = buff;
      buff = buff_sum;
      buff_sum = temp;
  	}
  	// reduction at inter group level
  	if( intraGroupRank == 0 && interGroupSize>1){
  		// printf("hi\n");
      
      MPI_Reduce(buff, buff_sum, D, MPI_DOUBLE, MPI_SUM, 0, interGroupLeaderComm);
  		double *temp = buff;
      buff = buff_sum;
      buff_sum = temp;
  	}

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