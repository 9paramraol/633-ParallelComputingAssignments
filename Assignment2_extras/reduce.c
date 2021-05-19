// Timing codes



#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"



int main( int argc, char *argv[])

{

  int myrank;

  int count = 2056*1024/sizeof(double);
  double *buff = (double*)malloc(sizeof(double)*count);
  double *buff_sum = (double*)malloc(sizeof(double)*count);
    

  int root = 1;

  MPI_Init( &argc, &argv );



  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );



  // initialize data

  for (int i=0; i<count; i++)

      buff[i] = myrank + i*i;



  // has to be called by all processes

  double sTime = MPI_Wtime();
  MPI_Reduce(buff, buff_sum, count, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);
  double eTime = MPI_Wtime() - sTime;


  double maxtime;
  MPI_Reduce(&eTime , &maxtime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if(!myrank)
    printf("%lf\n", maxtime);

  // simple check
  MPI_Finalize();

  return 0;



}


