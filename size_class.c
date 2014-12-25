/*
 * size_class.c
 *
 *  Created on: Dec 19, 2014
 *      Author: Nir Dothan 028434009
 */
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

unsigned short getFullness(superblock_t*);
superblock_t *removeSuperblock(size_class_t*, superblock_t*);
void printSuperblock(superblock_t *);
void printSizeClass(size_class_t *);

/*
void makeSizeClass(size_class_t *sizeClass, size_t size) {
	sizeClass->_sizeClassBytes = size;
	sizeClass->_SBlkList._length = 0;
	sizeClass->_SBlkList._first = NULL;
}*/

/* put a superblock between 2 exiting superblocks or as last */
void plantSuperBlock(superblock_t *pBefore, superblock_t *pNode,
		superblock_t *pAfter) {

	/* node points back to before */
	pNode->_meta._pPrvSblk = pBefore;

	/*node points to after - after can be NULL if node is last */
	pNode->_meta._pNxtSBlk = pAfter;

	/* before points to node */
	pBefore->_meta._pNxtSBlk = pNode;

	/* after points back to node */
	if (pAfter)
		pAfter->_meta._pPrvSblk = pNode;

	/* don't forget to increment zeClass->_SBlkList._length */

}

/*
 * insert a new superblock into the list
 *
 */
void insertSuperBlock(size_class_t *sizeClass, superblock_t *superBlock) {

	unsigned int i;
	superblock_t *pSb, *pPrevSb;




	/* first node - list is empty */
	if (sizeClass->_SBlkList._length == 0) {
		sizeClass->_SBlkList._length++;
		sizeClass->_SBlkList._first = superBlock;
		sizeClass->_SBlkList._first->_meta._pNxtSBlk = NULL;
		sizeClass->_SBlkList._first->_meta._pPrvSblk = NULL;
		sizeClass->_sizeClassBytes=superBlock->_meta._sizeClassBytes;

		return;

	}

	/* at least one superblock exists  */
	pPrevSb = NULL;
	pSb = sizeClass->_SBlkList._first;

	while (pSb && getFullness(superBlock) < getFullness(pSb)) {
		pPrevSb = pSb;
		pSb = pSb->_meta._pNxtSBlk;
	}

	/* either pSb is NULL (end of list) or new superblock is fuller than pSb  */
	if (!pSb) { /* end of list */

		if (pPrevSb == NULL) {
			printf("program bug! size_class.c cannot have prev==NULL \n ");
			exit(-1);
		}
		/* put superblock after pPrev as last*/
		plantSuperBlock(pPrevSb, superBlock, NULL);

	} else if (pSb == sizeClass->_SBlkList._first) {

		/* node points to existing first */
		superBlock->_meta._pNxtSBlk = sizeClass->_SBlkList._first;

		/* node points back to NULL */
		superBlock->_meta._pPrvSblk = NULL;

		/*  existing first points back at node */
		sizeClass->_SBlkList._first->_meta._pPrvSblk = superBlock;

		/*   first points to node */
		sizeClass->_SBlkList._first = superBlock;

	} else { /*  mid list */
		/* put superblock after pPrev and before pSb*/
		plantSuperBlock(pPrevSb, superBlock, pSb);
	}

	sizeClass->_SBlkList._length++;

}

/* find available superblock */

superblock_t *findAvailableSuperblock(size_class_t *sizeClass) {

	superblock_t *pSb = sizeClass->_SBlkList._first;
	unsigned int i;

	for (i=0; i< sizeClass->_SBlkList._length ; i++ ){
		if (pSb->_meta._NoFreeBlks > 0)
			return pSb;
		pSb=pSb->_meta._pNxtSBlk;
	}

	/* didn't find a free block */
	return NULL;
}

/* pop the least full superblock from the list
 *  would perform much better if we had a pointer to the last element (TBD) */
superblock_t *popLastSuperblock(size_class_t *sizeClass) {

	superblock_t *pSb = sizeClass->_SBlkList._first;

	if (!pSb) /*list empty*/
		return NULL;

	for (; pSb->_meta._pNxtSBlk; pSb = pSb->_meta._pNxtSBlk)
		;

	pSb->_meta._pPrvSblk->_meta._pNxtSBlk = NULL;
	pSb->_meta._pPrvSblk = NULL;

	sizeClass->_SBlkList._length--;

	return pSb;

}

/* relocate ahead:
 * move the superblock to its updated place after becoming less full i.e. a block was freed
 */
void relocateSuperBlockAhead(size_class_t *sizeClass, superblock_t *superBlock) {
	/* remove + insert is a temporary workaround -*/
	if (!sizeClass){
		printf("BUG! relocating a superblock without an owner \n");
		exit (-1);
	}
	removeSuperblock(sizeClass, superBlock);
	insertSuperBlock(sizeClass, superBlock);
}

/* relocate backward:
 * move the superblock to its updated place after becoming more full i.e. another block was allocated to the user
 */
void relocateSuperBlockBack(size_class_t *sizeClass, superblock_t *superBlock) {
	/* remove + insert is a temporary workaround -*/
	if (!sizeClass){
		printf("BUG! relocating a superblock without an owner \n");
		exit (-1);
	}
	removeSuperblock(sizeClass, superBlock);
	insertSuperBlock(sizeClass, superBlock);
}

/* remove  superblock from the list */
superblock_t *removeSuperblock(size_class_t *sizeClass,
		superblock_t *superBlock) {

	superblock_t *pPrevSb = superBlock->_meta._pPrvSblk;
	superblock_t *pNextSb = superBlock->_meta._pNxtSBlk;

	if (sizeClass->_SBlkList._first == superBlock) {/* it's first */

		/* first points to next */
		sizeClass->_SBlkList._first = pNextSb;

		/* if next exists, it points back to NULL */
		if (pNextSb)
			pNextSb->_meta._pPrvSblk = NULL;

	} else { /* not first */

		if (pPrevSb == NULL) {
			printf("program bug! size_class.c \n ");
			exit(-1);
		}
		/* prev points to next */
		pPrevSb->_meta._pNxtSBlk = pNextSb;

		/* if next exists point back to prev */
		if (pNextSb)
			pNextSb->_meta._pPrvSblk = pPrevSb;

	}

	sizeClass->_SBlkList._length--;
	superBlock->_meta._pNxtSBlk = superBlock->_meta._pPrvSblk = NULL;
	return superBlock;

}

void printSizeClass(size_class_t *sizeClass){
	int i;
	superblock_t *p=sizeClass->_SBlkList._first;
	printf("SizeClass [%d] # superblocks [%d]\n",sizeClass->_sizeClassBytes, sizeClass->_SBlkList._length);

	for(i=0;i< sizeClass->_SBlkList._length; i++, p=p->_meta._pNxtSBlk){
		printf("\n %d)  ",i);
		printSuperblock(p);
	}


}

