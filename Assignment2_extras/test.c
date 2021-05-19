#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "mpi.h"

int main (int argc, char *argv[]){
	
	// number of doubles
	int count =  atoi(argv[1]) / sizeof(double);
	int ppn =  atoi(argv[2]) ;
	MPI_Init ( &argc, &argv);
	
	int myrank;
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank );

	if(!myrank){
		printf("1\n");
		printf("2\n");
		printf("3\n");
		printf("4\n");
		printf("5\n");
		printf("6\n");
		printf("7\n");
		printf("8\n");
	}
	MPI_Finalize();
}



