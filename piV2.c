#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int i, done = 0, n, nproc, numprocs, rank;
    double PI25DT = 3.141592653589793238462643;
    double pi, h, sum, sumtotal, x;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    while(!done)
    {   if(rank == 0)
        {   printf("Enter the number of intervals: (0 quits) \n");
            scanf("%d",&n);        
        }

        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
        if(n == 0) break;
  
        h   = 1.0 / (double) n;
        sum = 0.0;
        for(i = 1+rank; i <= n; i += numprocs) 
        {   x = h * ((double)i - 0.5);
            sum += 4.0 / (1.0 + x*x);
        }

        MPI_Reduce(&sum, &sumtotal, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if(rank == 0)
        {   pi = h * sumtotal;
            printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        }
    }


    MPI_Finalize();
    return 0;
}
