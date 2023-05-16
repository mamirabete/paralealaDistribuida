#include <stdlib.h>
#include <stdio.h>

#define nprocs 2             /* numero de procesadores */

using namespace std;

extern "C" {
    #include <bsp.h>
}

//long int x,y;

int main()
{
  int x,y,i,sig; 
  bsp_begin(bsp_nprocs());
  bsp_push_reg(&y,sizeof(int));
  bsp_sync();
  x=bsp_pid()+1; 
  sig=(bsp_pid()+1)%(bsp_nprocs());
  printf("Procesador %d, valores de x:",bsp_pid());
   for(i=0; i<12; i++){
       bsp_put(sig, &x, &y, 0, sizeof(int));
       bsp_sync();
       x=x+y;
       printf(" %d ",x);     
      }
   printf("Sobre el Procesador %d la suma es %d\n",bsp_pid(),x);
   bsp_end(); 
   return 0;
}
