#include <stdio.h>
#include <math.h>
#include <mpi.h>

int MPI_BinomialBcast(void *argv, int argc, MPI_Datatype type, int root, MPI_Comm comm)
{
    int i = 0, rankOwn, rankDest, numprocs;
    MPI_Status status;

    root = 0;
    MPI_Comm_rank(comm, &rankOwn);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if(rankOwn != root)
    {   do
        {   MPI_Recv(argv, argc, type, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &status); 
            i = status.MPI_TAG;
        } while(log2(rankOwn - status.MPI_SOURCE) != i-1);
    }

    while((rankDest = rankOwn + (int) pow(2, i)) < numprocs)
    {   //printf("Paso %d: %d -> %d\n", i+1, rankOwn, rankDest);
        //fflush(NULL);
        i++;
        MPI_Send(argv, argc, type, rankDest, i, comm); //Se envia el nº de paso por el tag
    }

    return 0;
}

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

        MPI_BinomialBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
        if (n == 0) break;
  
        h   = 1.0 / (double) n;
        sum = 0.0;
        for (i = 1+rank; i <= n; i += numprocs) 
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
