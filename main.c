#include <stdio.h>
#include <strings.h>
#include "structs.h"

superblock_t* makeSuperblock(unsigned int );
void insertSuperBlock(size_class_t *sizeClass, superblock_t *superBlock) ;
void *getCore(unsigned int ) ;
void makeSizeClass(size_class_t *sizeClass, unsigned int size);
superblock_t *removeSuperblock(size_class_t *sizeClass,
		superblock_t *superBlock) ;

void relocateSuperBlockAhead(size_class_t *sizeClass, superblock_t *superBlock) ;

void main() {


char *a,*b,*c;





	    a=malloc(10);
	   b=malloc(10);
	    c=malloc(3);

	    freeme((void*)a);

		exit(0);





}
