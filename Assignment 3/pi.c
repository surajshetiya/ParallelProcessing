#include"mpi.h"
#include<stdio.h>
#include<math.h>
int main(argc,argv)
int argc;
char *argv[];
{
    int n, myid, numprocs, i;
        int done = 0;
    double PI25DT = 3.141592653589793238462643;
    double mypi, pi, h, sum, x, y;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    while (!done) {
        if (myid == 0)
        {
            printf("Enter the number of intervals: ");
            scanf("%d",&n);
        }
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (n <= 0) break;
                // If n is not even make it even
                n = n + n%2;

        h = 1.0 / (double) n;
        sum = 0.0;
        for (i = myid + 1; i <= n/2; i += numprocs)
        {
            x = h * ((double)(2*i-1));
            y = x + h;
            sum += 4.0 * 4.0 / (1.0 + x*x);
                        sum += 2.0 * 4.0 / (1.0 + y*y);
        }
        mypi = sum;
        MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (myid == 0){
                    pi += 4.0 - 2.0; // First set of elements 4/1 = 4 and 4/2 = 2
                pi = pi * h / 3.0;
            printf("pi approximately %.16f, Error is %.16f\n",pi,fabs(pi - PI25DT));
        }
        done = 1;
    }
    MPI_Finalize();
    return 0;
}
