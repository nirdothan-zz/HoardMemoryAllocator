/*#include <stdio.h>*/
#include <strings.h>
#include "memory_allocator.h"




void main() {


char *a,*b,*c;
int i;
void *arr[1000];
for (i=0; i<1000;i++){
	size_t sz=i%50+1;
	/*printf("%d allocating %u\n",i,sz);*/
	putchar('A');
	putchar('\n');
	arr[i]=(char*)malloc(sz);
	if (!arr[i])
	{
		printf("Error %d\n",i);
		exit(0);
	}


}
for (i=0; i<1000;i++){
	/*printf("%d freeing\n",i);*/
	putchar('D');
	putchar('\n');
	free(arr[i]);
}
printf("done\n");


/*

	    a=(char*)malloc(65536);
	   b=(char*)malloc(10);
	    c=(char*)malloc(10);

	    strcpy(a,"nir ");
	    strcpy(b,"dothan ");
	    strcpy(c,"is here ");

	    printf("%s %s %s\n",a,b,c);



	    free((void*)a);
	    printf("  %s %s\n",b,c);
	    free((void*)b);
	    printf("  %s\n",c);
	    free((void*)c);
*/
	    return;





}
