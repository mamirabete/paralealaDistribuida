#include <stdio.h>
#include <stdlib.h>

#define MAX 1000             /* longitud del arreglo */
#define nprocs 2             /* numero de procesadores */

using namespace std;

extern "C" {
    #include <bsp.h>
}

    int i,j,min,min2,sig,argc;
    int a[MAX];
    char **argv;
    
void bsp_main()
{
    bsp_begin(nprocs);
    for(i=0;i<MAX;i++) a[i]=(random()%1000000)+(3*bsp_pid());
    min=a[0];
    for(i=1;i<MAX;i++)
        if(a[i]<min) min=a[i];
    bsp_push_reg(&min2,sizeof(int));
    bsp_sync();
    sig=(bsp_pid()+1)%nprocs;
    for(i=0; i<(nprocs-1);i++) {
       bsp_put(sig,&min,&min2,0,sizeof(int));
       bsp_sync();
       if(min2<min) min=min2; }
    printf("procesador %d, minimo=%d\n",bsp_pid(),min);
    bsp_end(); 
}

int main (int _argc, char** _argv)
{
    void bsp_main();
    argc = _argc; argv = _argv;
    bsp_init(bsp_main,argc,argv);
    bsp_main();
    return 0;
}