// yesterday there was a mistake in get rank
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int safe_root(int a);
void fill_random(double *arr, int N);
void compute_inner(double *old, double *new, int N);
void copy(double *old, double *new, int N);
double find_new_for_point(int i, int j, int N, double *old, double *left, double *right, double *top, double *bottom);
void compute_edges(double *old, double *new, int N, int P, int my_row, int my_col, double *top, double *bottom, double *left, double *right);
int get_rank(int row, int col, int P);
int get_positn(int row, int col, int N);

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

  double *old = (double *)malloc(N*N*sizeof(double));
  double *new = (double *)malloc(N*N*sizeof(double));
  
  fill_random(old,N);

  double top_send_buff[N];
  double top_recv_buff[N]; 
  double top[N]; 
  MPI_Request top_send_request;
  MPI_Request top_recv_request;
  MPI_Status top_status;
  double bottom_send_buff[N]; 
  double bottom_recv_buff[N]; 
  double bottom[N]; 
  MPI_Request bottom_send_request;
  MPI_Request bottom_recv_request;
  MPI_Status bottom_status;
  double left_send_buff[N]; 
  double left_recv_buff[N]; 
  double left[N]; 
  MPI_Request left_send_request;
  MPI_Request left_recv_request;
  MPI_Status left_status;
  double right_send_buff[N]; 
  double right_recv_buff[N]; 
  double right[N]; 
  MPI_Request right_send_request;
  MPI_Request right_recv_request;
  MPI_Status right_status;

  int double_size;
  MPI_Type_size(MPI_DOUBLE, &double_size);
  // send from all ranks to the last rank
  start_time = MPI_Wtime ();
    
    int position;
    for(int t = 0; t<Timesteps; ++t){
        // sending to the left neg
        if((mycol-1)>=0){
            position = 0;
            for(int i=0;i<N;++i)
                MPI_Pack((old + get_positn(i,0,N)), 1, MPI_DOUBLE, left_send_buff, N*double_size, &position, MPI_COMM_WORLD);

            MPI_Isend(left_send_buff, position, MPI_PACKED, get_rank(myrow ,mycol-1, P), get_rank(myrow ,mycol-1, P),
            MPI_COMM_WORLD, &left_send_request);
              
            MPI_Irecv(left_recv_buff, N*double_size, MPI_PACKED, get_rank(myrow ,mycol-1, P), myrank,
            MPI_COMM_WORLD, &left_recv_request);
 
        }
        // sending to the right neg
        if((mycol+1)<P){
            position = 0;
            for(int i=0;i<N;++i)
                MPI_Pack((old + get_positn(i,N-1,N)), 1, MPI_DOUBLE, right_send_buff, N*double_size, &position, MPI_COMM_WORLD);

            MPI_Isend(right_send_buff, position, MPI_PACKED, get_rank(myrow ,mycol+1, P), get_rank(myrow ,mycol+1, P),
            MPI_COMM_WORLD, &right_send_request);
                
            MPI_Irecv(right_recv_buff, N*double_size, MPI_PACKED, get_rank(myrow ,mycol+1, P), myrank,
            MPI_COMM_WORLD, &right_recv_request);
        }
        // sending to the top neg
        if((myrow-1)>=0){
          position = 0;
          MPI_Pack(old, N, MPI_PACKED, top_send_buff, N*double_size, &position, MPI_COMM_WORLD);

          MPI_Isend(top_send_buff, position, MPI_PACKED, get_rank(myrow - 1,mycol, P), get_rank(myrow - 1,mycol, P),
          MPI_COMM_WORLD, &top_send_request);
                
          MPI_Irecv(top_recv_buff, N*double_size, MPI_PACKED, get_rank(myrow - 1,mycol, P), myrank,
          MPI_COMM_WORLD, &top_recv_request);

        }
        //sending to the bottom neg
        if((myrow+1)<P){
            position = 0;
            MPI_Pack((old+get_positn(N-1,0,N)), N, MPI_PACKED, bottom_send_buff, N*double_size, &position, MPI_COMM_WORLD);

            MPI_Isend(bottom_send_buff, position, MPI_PACKED, get_rank(myrow + 1,mycol, P), get_rank(myrow + 1,mycol, P),
            MPI_COMM_WORLD, &bottom_send_request);
                
            MPI_Irecv(bottom_recv_buff, N*double_size, MPI_PACKED, get_rank(myrow + 1,mycol, P), myrank,
            MPI_COMM_WORLD, &bottom_recv_request);
        }
        // computing the inner parts that don't need anything else
        compute_inner(old,new,N);
        
        //reciveing the right edge
        if((mycol-1)>=0){
            position = 0;
            MPI_Wait (&left_send_request, &left_status);
            MPI_Wait (&left_recv_request, &left_status);
            MPI_Unpack(left_recv_buff, N*double_size, &position, left, N, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        // sending to the right neg
        if((mycol+1)<P){
            position = 0;
            MPI_Wait (&right_send_request, &right_status);  
            MPI_Wait (&right_recv_request, &right_status); 
            MPI_Unpack(right_recv_buff, N*double_size, &position, right, N, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        // sending to the top neg
        if((myrow-1)>=0){
            position = 0;
            MPI_Wait (&top_send_request, &top_status);
            MPI_Wait (&top_recv_request, &top_status); 
            MPI_Unpack(top_recv_buff, N*double_size, &position, top, N, MPI_DOUBLE, MPI_COMM_WORLD); 
        }
        //sending to the bottom neg
        if((myrow+1)<P){
            position = 0;
            MPI_Wait (&bottom_send_request, &bottom_status);
            MPI_Wait (&bottom_recv_request, &bottom_status);  
            MPI_Unpack(bottom_recv_buff, N*double_size, &position, bottom, N, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        // updating the edges' elements
        compute_edges(old, new, N, P, myrow, mycol, top, bottom, left, right);
         double* temp = old;
          old = new;
          new = temp;
    }

  time = MPI_Wtime () - start_time;
  MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (!myrank) printf ("Max time = %lf\n", max_time);
  MPI_Finalize();
  return 0;
}

int safe_root(int a){
  int ans = 1;
  while(ans*ans != a)
      ++ans;
  return ans;
}

void fill_random(double *arr, int N){
    for(int i=0;i<N;++i){
      for(int j=0;j<N;++j)
          arr[get_positn(i,j,N)] = rand();
    }
    return;
}

void compute_inner(double *old, double *new, int N){
  for(int i=1;i<(N-1);++i){
    for(int j=1;j<(N-1);++j){
      double avg = (old[get_positn(i-1,j,N)] + old[get_positn(i+1,j,N)] + old[get_positn(i,j-1,N)] + old[get_positn(i,j+1,N)])/4.0;
      new[get_positn(i,j,N)] = avg;
    }
  }
  return;
}


//We are using this function for corner points only but it can work for any point
double find_new_for_point(int i, int j, int N, double *old, double *left, double *right, double *top, double *bottom){
    double neighbour_count = 0;
    double neighbour_sum = 0;
    // top point if exists
    if(i-1 >= 0){
        neighbour_sum += old[get_positn(i-1,j,N)];
        neighbour_count += 1;
    }
    else if(top != NULL){
        neighbour_sum += top[j];
        neighbour_count += 1;
    }

    // bottom point if exists
    if(i+1 <= (N-1)){
        neighbour_sum += old[get_positn(i+1,j,N)];
        neighbour_count += 1;
    }
    else if(bottom != NULL){
        neighbour_sum += bottom[j];
        neighbour_count += 1;
    }

    // left point if exists
    if(j-1 >= 0){
        neighbour_sum += old[get_positn(i,j-1,N)];
        neighbour_count += 1;
    }
    else if(left != NULL){
        neighbour_sum += left[i];
        neighbour_count += 1;
    }

    // right point if exists
    if(j+1 <= (N-1)){
        neighbour_sum += old[get_positn(i,j+1,N)];
        neighbour_count += 1;
    }
    else if(right != NULL){
        neighbour_sum += right[i];
        neighbour_count += 1;
    }

    return neighbour_sum/neighbour_count;
}


void compute_edges(double *old, double *new, int N, int P, int my_row, int my_col, double *top, double *bottom, double *left, double *right){
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
        new[get_positn(i,j,N)] = find_new_for_point(i, j, N, old, left, right, top, bottom);
        j = N-1;
        new[get_positn(i,j,N)] = find_new_for_point(i, j, N, old, left, right, top, bottom);
    }

    //finding for the top and the bottom row
    //Note that the computation for the corner points will be repeated but it does not make a lot of difference in terms of performance
    for(int j = 0; j < N; j++){
        int i = 0;
        new[get_positn(i,j,N)] = find_new_for_point(i, j, N, old, left, right, top, bottom);
        i = N-1;
        new[get_positn(i,j,N)] = find_new_for_point(i, j, N, old, left, right, top, bottom);
    }
    
    return;
}

int get_rank(int row, int col, int P){
    return row*P+col;
}

int get_positn(int row, int col, int N){
    return (row*N + col);
}