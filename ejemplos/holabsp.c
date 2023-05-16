#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <ctype.h>

using namespace std;

extern "C" {
  #include <bsp.h>
}

int pid, nprocs, argc;
char **argv;

void bsp_main()
{
	bsp_begin(bsp_nprocs());
        pid = bsp_pid();
        nprocs = bsp_nprocs();         
	printf("Hola BSP desde %d de %d 1\n", pid, nprocs);	
        bsp_sync();
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
