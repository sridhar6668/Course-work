#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "mpi.h"
#include <omp.h>
#include <math.h>
#include <cilk/cilk.h>

void MatrixMultiply(int n, int* X, int* Y, int* Z) {
    cilk_for (int i=0; i<n; i++) {
        for (int k=0; k<n; k++) {
            for (int j=0; j<n; j++) {        
                Z[i*n+j] += X[i*n+k] * Y[k*n+j];
            }
        }
    }
}

void printMatrix(int *A, int n)
{
	int i=0, j=0;
	for(i=0; i<n; i++)
	{
		for(j =0; j<n; j++)
		{
			printf("%d \t", A[i*n + j]);
		}
		printf("\n");
	}
	printf("\n");
}

//Print time elapsed in seconds
void print_time_elapsed(struct timeval* start, struct timeval* end) {
    
    /*
    struct timeval {
        time_t      tv_sec;
        suseconds_t tv_usec;    
    }*/
    struct timeval elapsed;
    
    if(start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec  = end->tv_sec  - start->tv_sec;
    float time_elapsed = (elapsed.tv_sec*1000000 + elapsed.tv_usec)/1000000.f;
    printf("Time taken: %f\n",time_elapsed);
} 

 
int main( int argc, char *argv[] )
{
	struct timeval start, end;
    struct timezone tzp;
    gettimeofday(&start, &tzp);
	
	int i, j, k,rank, proc_row, No_of_processors;
	MPI_Status stat;
	int sub_mat_size, mat_size;
	int processor_id;
	int x, y;
	int pineapple, position;
	int sending_to, receiving_from;
	
	MPI_Init(NULL, NULL);
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Comm_size(MPI_COMM_WORLD, &No_of_processors);
	MPI_Comm_rank(comm, &rank);
	for(pineapple = 10; pineapple <=14; pineapple++){
		if(rank == 0){
			printf("*********************************************************************************************************\n");
		}
		mat_size = pow(2,pineapple);
	proc_row = sqrt(No_of_processors);
	sub_mat_size = mat_size/proc_row;
	
	// Actual input matrix which will be used by the master
	int * First_matrix;
	int * Second_matrix;
	int * Output_matrix;
	int * temp_buf_A = (int *)malloc( sizeof(int)*(sub_mat_size*sub_mat_size) );
	int * temp_buf_B = (int *)malloc( sizeof(int)*(sub_mat_size*sub_mat_size) );
	int * temp_buf_C = (int *)malloc( sizeof(int)*(sub_mat_size*sub_mat_size) );
	// Matrix block which will be used for computation
	int * A = (int *)calloc(1, sizeof(int)*(sub_mat_size*sub_mat_size) );
	int * B = (int *)calloc(1, sizeof(int)*(sub_mat_size*sub_mat_size) );
	int * C = (int *)calloc(1, sizeof(int)*(sub_mat_size*sub_mat_size) );
	
	
	// Execute only if it is master
	if( rank == 0 ){
		// initialise the actual input n output matrices
		First_matrix = (int *)malloc( sizeof(int)*(mat_size*mat_size) );
		Second_matrix = (int *)malloc( sizeof(int)*(mat_size*mat_size) );	
		Output_matrix = (int *)malloc( sizeof(int)*(mat_size*mat_size) );
	
        for(i=0; i<mat_size*mat_size; i++){
			First_matrix[i] = i+1;
			Second_matrix[i] = i+1;
			Output_matrix[i] = 0;
		}
		//Copy the block that corresponds to the master only
		x = 0;
		y = 0;
		for(i =0; i<sub_mat_size; i++){
			for(j=0; j<sub_mat_size; j++){
				position = (x*sub_mat_size +i)*mat_size + (y*sub_mat_size + j);
				A[i*sub_mat_size + j] = First_matrix[ position];
				B[i*sub_mat_size + j] = Second_matrix[ position];
			}
		}
		//Create a temporary buffer and send the data block to each corresponding node.
		temp_buf_A = (int *)malloc( sizeof(int)*(sub_mat_size*sub_mat_size) );
		temp_buf_B = (int *)malloc( sizeof(int)*(sub_mat_size*sub_mat_size) );
			
		for (processor_id = 1; processor_id < No_of_processors; processor_id++){
			x = processor_id / proc_row;
			y = processor_id % proc_row;

			for(i =0; i<sub_mat_size; i++){
				for(j=0; j<sub_mat_size; j++){
					position = (x*sub_mat_size +i)*mat_size + (y*sub_mat_size + j);
					temp_buf_A[i*sub_mat_size + j] = First_matrix[ position ];
					temp_buf_B[i*sub_mat_size + j] = Second_matrix[ position ];
				}
			}
			MPI_Send(temp_buf_A, sub_mat_size*sub_mat_size , MPI_INT, processor_id, 0, MPI_COMM_WORLD);
			MPI_Send(temp_buf_B, sub_mat_size*sub_mat_size , MPI_INT, processor_id, 0, MPI_COMM_WORLD);
		}
	}
	else{
		// All other nodes receive data deom the master
		MPI_Recv(A, sub_mat_size*sub_mat_size, MPI_INT, 0, 0, MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
		MPI_Recv(B, sub_mat_size*sub_mat_size, MPI_INT, 0, 0, MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
	}
	
	// Compute the co-ordinates
	x = rank/proc_row;
	y = rank%proc_row;
	
	// Do the initial swapping
	sending_to = x*proc_row + ((y-x+proc_row)%proc_row);
	receiving_from = x*proc_row + (y+x)%proc_row;
	MPI_Sendrecv_replace(A, sub_mat_size*sub_mat_size, MPI_INT, sending_to, i, receiving_from, i, MPI_COMM_WORLD, &stat);
	sending_to = ((x-y+proc_row)%proc_row)*proc_row + y;
	receiving_from = ((x+y)%proc_row)*proc_row + y;
	MPI_Sendrecv_replace(B, sub_mat_size*sub_mat_size, MPI_INT, sending_to, i, receiving_from, i, MPI_COMM_WORLD, &stat);
	
	x = rank/proc_row;
	y = rank%proc_row;
	
	
	for(i = 0; i < proc_row; i++)	{	
		// Do matrix multiplication and swap the matrix blocks
		MatrixMultiply(sub_mat_size, A, B, C);
		sending_to = x*proc_row + (y-1+proc_row)%proc_row;
		receiving_from = x*proc_row + (y+1)%proc_row;
		MPI_Sendrecv_replace(A, sub_mat_size*sub_mat_size, MPI_INT, sending_to, i, receiving_from, i, MPI_COMM_WORLD, &stat);
		sending_to = ((x-1+proc_row)%proc_row)*proc_row + y;
		receiving_from = ((x+1)%proc_row)*proc_row + y;
		MPI_Sendrecv_replace(B, sub_mat_size*sub_mat_size, MPI_INT, sending_to, i, receiving_from, i, MPI_COMM_WORLD, &stat); 
	}





	if(rank == 0){
		// Finally the master will receive all the data		
		for (processor_id=1; processor_id<No_of_processors; processor_id++)
		{			
			x = processor_id / proc_row;
			y = processor_id % proc_row;
			
			//temp_buf_C = (int *)malloc( sizeof(int)*(sub_mat_size*sub_mat_size) );
			MPI_Recv(temp_buf_C, sub_mat_size*sub_mat_size, MPI_INT, processor_id, 5, MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
			
			for(i =0; i<sub_mat_size; i++)
			{
				for(j=0; j<sub_mat_size; j++)
				{
					Output_matrix[ (x*sub_mat_size +i)*mat_size + (y*sub_mat_size + j)] = temp_buf_C[i*sub_mat_size + j];
				}
			}
		}
		
		x = 0;
		y = 0;
		for(i =0; i < sub_mat_size; i++)
		{
			for(j=0; j < sub_mat_size; j++)
			{
				Output_matrix[ (x*sub_mat_size +i)*mat_size + (y*sub_mat_size + j)] = C[i*sub_mat_size + j];
			}
		}
		

		gettimeofday(&end, &tzp);
		print_time_elapsed(&start, &end);
		printf("Matrix size: %d, Processors: %d\n", mat_size, No_of_processors);
	}
	else
	{
		// Each node will send the output block to the master
		MPI_Send(C, sub_mat_size*sub_mat_size , MPI_INT, 0, 5, MPI_COMM_WORLD);
	}
	/*
	free(First_matrix);
	free(Second_matrix);
	free(Output_matrix);
	free(A);
	free(B);
	free(C);
	free(temp_buf_A);
	free(temp_buf_B);
	free(temp_buf_C);
	*/
	}
	
	MPI_Finalize();
	return 0;
}


