// Timing codes



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



int main( int argc, char *argv[])

{
  
  // 1. make subcommunicators for all the process in a single group
  // 2. designated the processes with rank 0 in each intranode group 
  //    as a leader.
  // 3. make a subcommunicator of all the leaders
  // 4. If there is only group simply broadcast as the further steps
  //    are not need
  // 5. from the root of the bcast send the buffer to the leader of 
  //    the group to which the root belongs.
  //    (if the root is the leader no need to send to itself otherwise
  //     it may deadlock).
  // 6. from the root's group leader bcast to all the leaders of the groups, 
  // 	note that if the code reaches here the that means that there
  // 	are multiple leaders.
  // 7. now from each leader if the leader is not the only member 
  //    in its group broadcast from it to the whole group.
  
  
  int root = 0;
  int D = 1000000;
  int buf[D];
  char hostname[MPI_MAX_PROCESSOR_NAME];

  // initialize MPI
  MPI_Init (&argc, &argv);

  int numProc, rank, processorNameLen;

  // get number of processes
  MPI_Comm_size (MPI_COMM_WORLD, &numProc);

  // get my rank
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  //initialize the buffer the from the root
  if(rank == root){
    for(int i = 0; i<D;i++){
      buf[i] = i;
    }
  }

  double sTime = MPI_Wtime();
  // get the name of the processor
  MPI_Get_processor_name (hostname, &processorNameLen);

  //getting the groupNumber
  int groupNumber =  getGroupNumberFromHostname(hostname);

  // 1. making a subcommunicator from the processes in a single group
  // using the groupNumber as the color.
  int color = groupNumber;
  int intraGroupRank, intraGroupSize;
  MPI_Comm intraGroupComm;
  MPI_Comm_split (MPI_COMM_WORLD, color, rank, &intraGroupComm);
  // getting the intraGroupRank
  MPI_Comm_rank( intraGroupComm, &intraGroupRank );
  // getting the intragGroupSize
  MPI_Comm_size( intraGroupComm, &intraGroupSize );
  
  // 2 and 3. making the a subcommicator out of the leaders
  MPI_Comm interGroupLeaderComm;
  if(intraGroupRank == 0){
    //giving the colour 0 as the colour does not really matter here
    MPI_Comm_split (MPI_COMM_WORLD, 0, rank, &interGroupLeaderComm);
  }
  else{
    // the other process also need to call it but since they dont need
    // to be part of this communicator we keep the color as 
    // MPI_UNDEFINED.
    MPI_Comm_split (MPI_COMM_WORLD, MPI_UNDEFINED, rank, &interGroupLeaderComm);
  }

  // 4. find the number of group and broadcast from the 0th process 
  //    to every other if there is only one group
  int numGroups;
  // note that this call will not work for the processes which 
  // are not in this process and hence you will get a null communicato
  // error, hence use these calls carefully
  

  //here is a bug that intragroup size is not being communicated
  //to all processes, and hence the numgroups == 1 check is not
  // going through, which means that there is a bug, to fix it 
  // we will broadcast the numgourps variable from all the leaders to 
  // the groups.
  if(intraGroupRank == 0){
    // this check is because only the leaders belong to this communicator
    MPI_Comm_size( interGroupLeaderComm, &numGroups );
  }
  
  if(intraGroupSize > 1){
    MPI_Bcast(&numGroups, 1, MPI_INT, 0, intraGroupComm);
  }

 
  if(numGroups == 1){
    if(numProc > 1){
      // the D(size of the buffer) and root(root for bcast) are kept 
      // variable to test the algo more rigourously
      MPI_Bcast(buf, D, MPI_INT, root, MPI_COMM_WORLD);
    }
  }
  else{//there are more than 1 groups
    //note that this must from the root's group only make 
    //that correction, to make that correction broadcast the root's
    //group or colour, to every process.
    
    int rootGroup;
    if(rank == root){
      rootGroup =  getGroupNumberFromHostname(hostname);
    }
    //broadcasting the rootGroup to each process, so that we can use
    //it
    MPI_Bcast(&rootGroup, 1, MPI_INT, root, MPI_COMM_WORLD);
    if(rank == root && intraGroupRank != 0 && color == rootGroup){
      //if the process is the root of the bcast and the process is not
      //the leader of the group, send to the leader of the group
      MPI_Send (buf, D, MPI_INT, 0, 1, intraGroupComm);
    }
    else if(rank != root && intraGroupRank == 0 && color == rootGroup){
      MPI_Status status;
      // note that we can easily use MPI_ANY_SOURCE as there will be 
      // only one send in the entire  algorithm
      MPI_Recv(buf, D, MPI_INT, MPI_ANY_SOURCE, 1, intraGroupComm, &status);
    }


    // now we will broadcast from the root group's leader to all the 
    // groups leaders.But first we will need to broadcast the rank 
    // of the root group leader so the respect to the inter group 
    // leader communicator that all the group leaders can 
    // bcast correctly
    
    int rootGroupLeaderRank;
    if(color == rootGroup && intraGroupRank == 0){
      rootGroupLeaderRank = MPI_Comm_rank( interGroupLeaderComm, &intraGroupRank );
    }

    if(rank == root && intraGroupRank != 0 && color == rootGroup){
      //if the process is the root of the bcast and the process is not
      //the leader of the group, receive from the leader of the group
      MPI_Status status;
      MPI_Recv (&rootGroupLeaderRank, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
    }
    else if(rank != root && intraGroupRank == 0 && color == rootGroup){
      MPI_Send(&rootGroupLeaderRank, 1, MPI_INT, root, 1, MPI_COMM_WORLD);
    }

    MPI_Bcast(&rootGroupLeaderRank, 1, MPI_INT, root, MPI_COMM_WORLD);
    if(intraGroupRank == 0){
      MPI_Bcast(buf, D, MPI_INT, rootGroupLeaderRank, interGroupLeaderComm);
    }

    if(intraGroupSize > 1){
      MPI_Bcast(buf, D, MPI_INT, 0, intraGroupComm);
    }

    
  }

  double eTime = MPI_Wtime();

  // done with MPI
  MPI_Finalize();
  
  printf ("%d %lf\n", rank, eTime - sTime);
  
  //for(int i = 0; i<D;i++){
  //  printf("%d ", buf[i]);
  //}
  //printf("\n");


  return 0;



}

