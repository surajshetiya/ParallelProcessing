#include"mpi.h"
#include<stdio.h>
#include<math.h>

int main(int argc, char *argv[]) {

        int myid, numprocs;
        long n;
        MPI_Init(&argc,&argv);
        MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD,&myid);
        if(myid == 0) {
                printf("\nEnter the value of n :");
                scanf("%ld",&n);
        }
        MPI_Bcast(&n, 1, MPI_LONG, 0, MPI_COMM_WORLD);
        long per_processor = (long)n/numprocs;
        long start_value = per_processor*(myid)+1;
        long end_value = per_processor*(myid+1);
        if(myid == numprocs-1) {
                end_value = n;
        }
        long total_no = end_value - start_value + 1,i=0;
        int primes[total_no];
        for(i=0;i<total_no;i++){
                primes[i] = 0;
        }
        int done = 0;
        double time_taken = 0;
        long total_primes = 0;
        long current_prime = 2;
        long largest_prime = sqrt(n);
        long current_p;
        double time_taken_communication = 0;
        MPI_Barrier(MPI_COMM_WORLD);
        time_taken = -MPI_Wtime();
        while(!done) {
                time_taken_communication += -MPI_Wtime();
                MPI_Bcast(&current_prime, 1, MPI_LONG, 0, MPI_COMM_WORLD);
                time_taken_communication += MPI_Wtime();
                if(current_prime == -1) {
                        done = 1;
                        break;
                }
                current_p = current_prime;
                long marking_value = current_prime - (start_value % current_prime) + start_value;
                if(current_prime * current_prime > marking_value) {
                        marking_value = current_prime*current_prime;
                }
                if(current_prime != 2){
                        if(marking_value%2 == 0){
                                marking_value += current_prime;
                        }
                        current_p *= 2;
                }
                while(marking_value <= end_value) {
                        primes[marking_value - start_value] = 1;
                        marking_value = marking_value + current_p;
                }
                if(myid == 0){
                        long temp = current_prime+1;
                        if(temp % 2 == 0){
                                temp = temp+1;
                        }
                        for(;temp <= largest_prime ;temp+=2){
                                if(primes[temp-1] == 0) {
                                        break;
                                }
                        }
                        if(temp <= largest_prime){
                                current_prime = temp;
                        } else {
                                current_prime = -1;
                        }
                }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        time_taken += MPI_Wtime();
        if(myid == 0){
                printf("\nTime taken = %f\n",time_taken);
        }
        int barrier_count;
        for(barrier_count = 0; barrier_count < myid; barrier_count++) {
                MPI_Barrier(MPI_COMM_WORLD);
        }
        printf("\nPRocessor %d printing primes",myid+1);
        long temp_value;
        for(temp_value=start_value;temp_value < end_value;temp_value++){
                if(primes[temp_value - start_value] == 0){
                        printf("%ld\n",temp_value);
                }
        }
        for(; barrier_count < numprocs; barrier_count++) {
                MPI_Barrier(MPI_COMM_WORLD);
        }
        MPI_Finalize();
}
