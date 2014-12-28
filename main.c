#include <stdio.h>
#include <strings.h>
#include "memory_allocator.h"




void main() {


char *a,*b,*c;





	    a=(char*)malloc(65536);
	   b=(char*)malloc(10);
	    c=(char*)malloc(10);

	    strcpy(a,"nir ");
	    strcpy(b,"dothan ");
	    strcpy(c,"is here ");

	    printf("%s %s %s\n",a,b,c);



	    myfree((void*)a);
	    printf("  %s %s\n",b,c);
	    myfree((void*)b);
	    printf("  %s\n",c);
	    myfree((void*)c);

	    return;





}
