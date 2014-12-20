/*
 * size_class.c
 *
 *  Created on: Dec 19, 2014
 *      Author: student
 */
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

unsigned short getFullness(superblock_t*);

void makeSizeClass(size_class_t *sizeClass, unsigned int size) {
	sizeClass->_sizeClass = size;
	sizeClass->_SBlkList._length = 0;
	sizeClass->_SBlkList._first = NULL;
}

/* put a superblock between 2 exiting superblocks or as last */
void plantSuperBlock( superblock_t *pBefore,  superblock_t *pNode,
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

void insertSuperBlock( size_class_t *sizeClass, superblock_t *superBlock) {

	unsigned int i;
	superblock_t *pSb, *pPrevSb;

	/* first node - list is empty */
	if (sizeClass->_SBlkList._length == 0) {
		sizeClass->_SBlkList._length++;
		sizeClass->_SBlkList._first = superBlock;
		sizeClass->_SBlkList._first->_meta._pNxtSBlk = NULL;
		sizeClass->_SBlkList._first->_meta._pPrvSblk = NULL;

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
			printf("program bug! size_class.c \n ");
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
