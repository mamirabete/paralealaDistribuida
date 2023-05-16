#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/times.h>
#include <sys/time.h>
//#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <iomanip>

int main(int argc, char **argv) {
   int n, myid, numprocs, i;
   double mypi, pi, h, sum, x;

   // VARIABLES PARA CONTEO DE TIEMPOS
   struct timeval tv, t1, t2;
   struct timezone tz;
   int diferencia, dif1, dif2;
   time_t inicio,fin;

   MPI_Init (&argc,&argv);
   MPI_Comm_size (MPI_COMM_WORLD,&numprocs);
   MPI_Comm_rank (MPI_COMM_WORLD,&myid);

   if (myid == 0) { printf("Numero de intervalos: "); 
	scanf("%d",&n); 
   }
   gettimeofday(&t1,NULL);
   inicio=time(NULL);

   MPI_Bcast (&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
   h = 1.0 / (double) n; sum = 0.0;

   for (i = myid + 1; i <= n; i += numprocs)
	{ x = h * ((double)i - 0.5);
	sum += 4.0 / (1.0 + x*x);
	
        }   

   mypi = h * sum;
   MPI_Reduce (&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

   if (myid == 0) {
   
      printf("pi -aproximado- %.16f, \n", pi);
      gettimeofday(&t2,NULL);
      dif1 = (t1.tv_sec*1000000)+t1.tv_usec;
      dif2 = (t2.tv_sec*1000000)+t2.tv_usec;
      diferencia = dif2 - dif1;
      fin=time(NULL);
      printf("Tardé %d microsegundos \n",diferencia);
      printf("Tardé %f segundos \n",difftime(fin,inicio));
    }
  
   MPI_Finalize();
   return 0;
}

