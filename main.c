#include <stdio.h>
#include <strings.h>
#include "hoard.h"

superblock_t* makeSuperblock(unsigned int );

void *getCore(unsigned int ) ;
void main() {




	superblock_t *sb = makeSuperblock(20000);
	block_header_t *bl2, *bl=popBlock(sb);


	bl2=popBlock(sb);

	pushBlock(sb,bl2);


	printf("done\n");
	getchar();


}
