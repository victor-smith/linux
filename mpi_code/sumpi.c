#include <stdio.h>
#include <mpi.h>
#include <math.h>

void main(int argc, char ** argv)
{
	const double pi = acos(-1.0);
	double dx, xi, mypi, tpi;
	int myid, np, n, i;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	do
	{
		if(myid == 0)
		{
			printf("input the N:");
			scanf("%d",&n);
		}

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if(n <= 0) break;

		dx = 1.0/n;
		mypi = 0.0;
		for(i=myid+1; i<=n; i+=np)
		{
			xi = (i-0.5)/n;
			mypi += 4.0/(1+xi*xi)*dx;
		}

		MPI_Reduce(&mypi,&tpi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		if(myid==0)
		{
			printf("   pi:%.16f\n", pi);
			printf("  tpi:%.16f\n", tpi);
			printf("ratio:%.16e%%\n", fabs(pi-tpi)/pi*100);
		}
	}while(0);
	MPI_Finalize();
}
