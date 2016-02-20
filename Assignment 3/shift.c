#include"mpi.h"
#include<stdio.h>
#include<math.h>

int size_array;

void SHIFT(int act_array[], int final_array[], int shift_factor) {
	int myid, numprocs;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Request request;

	int temp_val = 0;
	for(;temp_val<size_array;temp_val++) {
		printf("\nProcessor %d : Non shifted element %d is %d",myid+1, temp_val+1,act_array[temp_val]);
		final_array[temp_val] = act_array[temp_val];
	}
	if(myid == 0) {
		
		if(shift_factor > 0){
                        int send_to = (myid + 1)%numprocs;
                        int recv_from = numprocs -1;
			int i=0;
			for(;i<shift_factor;i++){
				int my_local_id = myid;
				int temp=final_array[0],j;
	                        MPI_Isend(&final_array[size_array-1], 1, MPI_INT, send_to, send_to, MPI_COMM_WORLD, &request) ;
        	                MPI_Recv(&final_array[0], 1 , MPI_INT, recv_from, my_local_id, MPI_COMM_WORLD, &status);
				for(j=size_array-1;j>1;j--){
					final_array[j] = final_array[j-1];
				}
				final_array[j] = temp;
			}
		} else {
			int i=0;
			int send_to = numprocs-1;
			int recv_from = (myid + 1)%numprocs;
			for(;i>shift_factor;i--){
				int my_local_id = myid;
				int temp=final_array[size_array-1],j;
				MPI_Isend(&final_array[0], 1, MPI_INT, send_to, send_to, MPI_COMM_WORLD, &request);
				MPI_Recv(&final_array[size_array-1], 1, MPI_INT, recv_from, my_local_id, MPI_COMM_WORLD, &status);
				for(j=0; j < size_array-2 ; j++){
					final_array[j] = final_array[j+1];
				}
				final_array[j] = temp;
			}
		}
	}
	else {
		if(shift_factor > 0){
			int send_to = (myid + 1)%numprocs;
			int recv_from = myid - 1;
			int i;
			for(i=0; i < shift_factor ; i++){
				int my_local_id = myid;
				int temp=final_array[0],j;
				MPI_Recv(&final_array[0], 1, MPI_INT, recv_from, my_local_id, MPI_COMM_WORLD, &status);
				MPI_Send(&final_array[size_array-1], 1, MPI_INT, send_to, send_to, MPI_COMM_WORLD);
				for(j=size_array-1;j>1;j--){
                                        final_array[j] = final_array[j-1];
                                }
				final_array[j] = temp;
			}
		} else {
			int send_to = myid - 1;
                        int recv_from = (myid + 1)%numprocs;
			int fabs_shift_factor = -shift_factor;
			int i = 0;
			for(; i > shift_factor; i--) {
				int my_local_id = myid;
				int temp=final_array[size_array-1],j;
				MPI_Recv(&final_array[size_array-1], 1, MPI_INT, recv_from, my_local_id, MPI_COMM_WORLD, &status);
				MPI_Send(&final_array[0], 1, MPI_INT, send_to, send_to, MPI_COMM_WORLD);
				for(j=0; j < size_array-2 ; j++){
                                        final_array[j] = final_array[j+1];
                                }
				final_array[j] = temp;
			}
		}
	}
	for(temp_val=0;temp_val<size_array;temp_val++) {
		printf("\nProcessor %d : Shifted element %d is %d",myid+1, temp_val+1,final_array[temp_val]);
	}
}

int main(int argc,char *argv[])
{
	int myid, numprocs, shift_factor;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	if(myid == 0) {
		printf("\nEnter the size of the array : ");
		scanf("%d",&size_array);
	}
	MPI_Bcast(&size_array, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int actual_array[size_array], final_array[size_array];

	// Reading data
	if(myid == 0) {
		int i;
		printf("\nProcessor no %d, enter %d elements ",myid+1,size_array);
		for(i=0; i<size_array; i++) {
			scanf("%d",&actual_array[i]);
		}
		int j=0;
		for(;j<numprocs;j++){
			MPI_Barrier(MPI_COMM_WORLD);
		}
	} else {
		MPI_Status status;
		int i;
		int j=0;
		for(;j<myid;j++){
			MPI_Barrier(MPI_COMM_WORLD);
		}
		printf("\nProcessor no %d, enter %d elements ",myid+1,size_array);
                for(i=0; i<size_array; i++) {
                        scanf("%d",&actual_array[i]);
                }
		for(;j<numprocs;j++){
                        MPI_Barrier(MPI_COMM_WORLD);
                }
	}

	// Reading shift factor
	if(myid == 0) {
		printf("\nEnter the value of shift factor : ");
		scanf("%d",&shift_factor);
	}
	MPI_Bcast(&shift_factor, 1, MPI_INT, 0, MPI_COMM_WORLD);
	SHIFT(actual_array, final_array, shift_factor%(numprocs*size_array));

	MPI_Finalize();
	return 0;
}

