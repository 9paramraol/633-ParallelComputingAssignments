// yesterday there was a mistake in get rank
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int safe_root(int a){
  int ans = 1;
  while(ans*ans != a)
      ++ans;
  return ans;
}

void fill_random(double **arr, int N){
    for(int i=0;i<N;++i){
      for(int j=0;j<N;++j)
          arr[i][j] = (double)((int)rand()%10);
    }
    return;
}

void compute_inner(double **old, double **new, int N){
  for(int i=1;i<(N-1);++i){
    for(int j=1;j<(N-1);++j){
      double avg = (old[i-1][j] + old[i+1][j] + old[i][j+1] + old[i][j-1])/4.0;
      new[i][j] = avg;
    }
  }
  return;
}

void copy(double **old, double **new, int N){
  for(int i=0;i<N;++i){
      for(int j=0;i<N;++i){
          new[i][j] = old[i][j];
      }
  }
  return;
}



//We are using this function for corner points only but it can work for any point
double find_new_for_point(int i, int j, int N, double **old, double *left, double *right, double *top, double *bottom){
    double neighbour_count = 0;
    double neighbour_sum = 0;
    // top point if exists
    if(i-1 >= 0){
        neighbour_sum += old[i-1][j];
        neighbour_count += 1;
    }
    else if(top != NULL){
        neighbour_sum += top[j];
        neighbour_count += 1;
    }

    // bottom point if exists
    if(i+1 <= (N-1)){
        neighbour_sum += old[i+1][j];
        neighbour_count += 1;
    }
    else if(bottom != NULL){
        neighbour_sum += bottom[j];
        neighbour_count += 1;
    }

    // left point if exists
    if(j-1 >= 0){
        neighbour_sum += old[i][j-1];
        neighbour_count += 1;
    }
    else if(left != NULL){
        neighbour_sum += left[i];
        neighbour_count += 1;
    }

    // right point if exists
    if(j+1 <= (N-1)){
        neighbour_sum += old[i][j+1];
        neighbour_count += 1;
    }
    else if(right != NULL){
        neighbour_sum += right[i];
        neighbour_count += 1;
    }

    return neighbour_sum/neighbour_count;
}


void compute_edges(double **old, double **new, int N, int P, int my_row, int my_col, double *top, double *bottom, double *left, double *right){
    if(my_col == 0){
        //assigning these to null so that if they are ever dereferenced here we know that there is an error
        left = NULL;
    }
    if(my_col == P-1){
        //assigning these to null so that if they are ever dereferenced here we know that there is an error
        right = NULL;
    }
    if(my_row == 0){
        //assigning these to null so that if they are ever dereferenced here we know that there is an error
        top = NULL;
    }
    if(my_row == P-1){
        //assigning these to null so that if they are ever dereferenced here we know that there is an error
        bottom = NULL;
    }

    //Now we will find neighbours for each point and update them

    //finding for the left and the right colums
    for(int i = 0; i < N; i++){
        int j = 0;
        new[i][j] = find_new_for_point(i, j, N, old, left, right, top, bottom);
        j = N-1;
        new[i][j] = find_new_for_point(i, j, N, old, left, right, top, bottom);
    }

    //finding for the top and the bottom row
    //Note that the computation for the corner points will be repeated but it does not make a lot of difference in terms of performance
    for(int j = 0; j < N; j++){
        int i = 0;
        new[i][j] = find_new_for_point(i, j, N, old, left, right, top, bottom);
        i = N-1;
        new[i][j] = find_new_for_point(i, j, N, old, left, right, top, bottom);
    }
    
    return;
}

int get_rank(int row, int col, int P){
    return row*P+col;
}


void print_in_file(FILE **fp, int myrow, int mycol, int N, int t, double** old){
    int orow = myrow * N;
    int ocol = mycol *N;
    for(int r=0; r < N; ++r){
      for(int c=0; c < N; ++c)
        fprintf(*fp, "%d,%d,%d,%lf\n", t, (orow + r), (ocol + c), old[r][c]);
    }
    return;
}

int main( int argc, char *argv[])
{
  int N = atoi(argv[1]);
  int Timesteps = atoi(argv[2]);

  int myrank, P;
  double start_time, time, max_time;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
  MPI_Comm_size( MPI_COMM_WORLD, &P );
  // MPI_Status status[size-1];
  
  P = safe_root(P);
  N = safe_root(N);
  int myrow = myrank/P;
  int mycol = myrank%P;

  

  double *old[N];
  for(int i=0; i<N; ++i)
    old[i] = (double *)malloc(N*sizeof(double));
  double *new[N];
  for(int i=0;i<N;++i)
    new[i] = (double *)malloc(N*sizeof(double));

  fill_random(old,N);
  
  double top[N]; 
  MPI_Request top_send_request[N];
  MPI_Request top_recv_request[N];
  MPI_Status top_status[N];
  double bottom[N]; 
  MPI_Request bottom_send_request[N];
  MPI_Request bottom_recv_request[N];
  MPI_Status bottom_status[N];
  double left[N]; 
  MPI_Request left_send_request[N];
  MPI_Request left_recv_request[N];
  MPI_Status left_status[N];
  double right[N]; 
  MPI_Request right_send_request[N];
  MPI_Request right_recv_request[N];
  MPI_Status right_status[N];
  // send from all ranks to the last rank
  start_time = MPI_Wtime ();
  
  // for the file where to write  
  FILE *filePointer;
  char fname[30];  sprintf(fname, "./all_ranks/%d", myrank);
  filePointer = fopen(fname, "w");
  //print_in_file(&filePointer, myrow, mycol, N, t, old);

    for(int t = 0; t<Timesteps; ++t){
        // sending to the left neg
        print_in_file(&filePointer, myrow, mycol, N, t, old);
        if((mycol-1)>=0){
            for(int i=0;i<N;++i){
                // printf("left %d:isent\n",myrank);
                MPI_Isend((old[i] + 0), 1, MPI_DOUBLE, get_rank(myrow ,mycol-1, P), get_rank(myrow ,mycol-1, P),
                MPI_COMM_WORLD, &left_send_request[i]);
              
                MPI_Irecv(&left[i], 1, MPI_DOUBLE, get_rank(myrow ,mycol-1, P), myrank,
                MPI_COMM_WORLD, &left_recv_request[i]);
            }

        }
        // sending to the right neg
        if((mycol+1)<P){
            for(int i=0;i<N;++i){
                MPI_Isend((old[i] + (N-1)), 1, MPI_DOUBLE, get_rank(myrow ,mycol+1, P), get_rank(myrow ,mycol+1, P),
                MPI_COMM_WORLD, &right_send_request[i]);
                
                MPI_Irecv(&right[i], 1, MPI_DOUBLE, get_rank(myrow ,mycol+1, P), myrank,
                MPI_COMM_WORLD, &right_recv_request[i]);
            }
        }
        // sending to the top neg
        if((myrow-1)>=0){
            for(int i=0;i<N;++i){
                MPI_Isend((old[0] + i), 1, MPI_DOUBLE, get_rank(myrow - 1,mycol, P), get_rank(myrow - 1,mycol, P),
                MPI_COMM_WORLD, &top_send_request[i]);
                
                MPI_Irecv(&top[i], 1, MPI_DOUBLE, get_rank(myrow - 1,mycol, P), myrank,
                MPI_COMM_WORLD, &top_recv_request[i]);
            }

        }
        //sending to the bottom neg
        if((myrow+1)<P){
            // there was an error here instead of P N was being used, both in the above check and the get rank function
            for(int i=0;i<N;++i){
                MPI_Isend((old[N-1] + i), 1, MPI_DOUBLE, get_rank(myrow + 1,mycol, P), get_rank(myrow + 1,mycol, P),
                MPI_COMM_WORLD, &bottom_send_request[i]);
                
                MPI_Irecv(&bottom[i], 1, MPI_DOUBLE, get_rank(myrow + 1,mycol, P), myrank,
                MPI_COMM_WORLD, &bottom_recv_request[i]);
            }
       
        }
        // // computing the inner parts that don't need anything else
        compute_inner(old,new,N);
        
        //reciveing the right edge
        if((mycol-1)>=0){
            MPI_Waitall (N, left_send_request, left_status);
            MPI_Waitall (N, left_recv_request, left_status);
        }
        // sending to the right neg
        if((mycol+1)<P){
            MPI_Waitall (N, right_send_request, right_status);  
            MPI_Waitall (N, right_recv_request, right_status); 
        }
        // sending to the top neg
        if((myrow-1)>=0){
            MPI_Waitall (N, top_send_request, top_status);
            MPI_Waitall (N, top_recv_request, top_status);  
        }
        //sending to the bottom neg
        if((myrow+1)<P){
            MPI_Waitall (N, bottom_send_request, bottom_status);
            MPI_Waitall (N, bottom_recv_request, bottom_status);  
        }
        
        

        // updating the edges' elements
        compute_edges(old, new, N, P, myrow, mycol, top, bottom, left, right);
        //note that implementing the compute edges as 4 updates and having the waitalls in between instead of together might be more efficient
        // and it is also not complicated due to how we have implemented the function find_new_for_point

        

        //switching the pointer of the old and the new arrar instead of copying them
        
    
        for(int i=0;i<N;++i){
          double* temp = old[i];
          old[i] = new[i];
          new[i] = temp;
        }

        // continuing on with the next iteration
    }

  time = MPI_Wtime () - start_time;
  MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (!myrank) printf ("Max time = %lf\n", max_time);
  MPI_Finalize();
  return 0;
}
