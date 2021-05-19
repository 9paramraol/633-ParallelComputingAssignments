 
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "mpi.h"

// an array where the the xth index of the csewsx = the group to which it belongs. The groups start at 0. 
int nodeToGroupMap[93] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

// takes the starting of the hostname string and returns the groups to which the nodes belongs to.
int getGroupNumberFromHostname(char* hostname){
  int csewsNumber = atoi(hostname+5);
  return nodeToGroupMap[csewsNumber];
}

int main(int argc, char *argv[]) 
{
  int numtasks, rank, len, coreID;
  char hostname[MPI_MAX_PROCESSOR_NAME];

  // initialize MPI
  MPI_Init (&argc, &argv);

  // get number of tasks
  MPI_Comm_size (MPI_COMM_WORLD, &numtasks);

  // get my rank
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  // this one is obvious
  MPI_Get_processor_name (hostname, &len);
  
  // getting the number x from csewsx
  int csewsNumber = atoi(hostname+5);

  //getting the groupNumber
  int groupNumber =  getGroupNumberFromHostname(hostname);

  // core ID
  coreID = sched_getcpu();

  printf ("%d of %d Running on %s:%d with csews number %d in  the group %d\n", rank, numtasks, hostname, coreID, csewsNumber, groupNumber);

  // done with MPI
  MPI_Finalize();
}

