/*
 * cpu_heap.c
 *
 *  Created on: Dec 23, 2014
 *     Author: Nir Dothan 028434009
 *
 *     This module implements functions that performs various operations at a single CPU heap level
 */


#include "memory_allocator.h"

/* remove a superblock from a given heap and sizeclass index and update heap level stats */
void removeSuperblockFromHeap(cpuheap_t *heap, int sizeClass_ix, superblock_t *pSb){

	size_class_t *pSizeClass=&(heap->_sizeClasses[sizeClass_ix]);

	if(!pSizeClass){
		printf("NULL size class\n");
		exit(-1);
	}
	removeSuperblock(pSizeClass,pSb);

	/* disown the superblock */
	pSb->_meta._pOwnerHeap=NULL;


	/*
	 *
 11. u 0 ← u 0 + s1.u,
 u i ← u i − s1.u
 12. a 0 ← a 0 + S,
 a i ← a i − S
	 *
	 *
	 */

	/* update heap level stats */

	/* 11. u 0 ← u 0 − s.u */
	heap->_bytesUsed-=getBytesUsed(pSb);
	/* step #13. a 0 ← a 0 − S */
	heap->_bytesAvailable-=SUPERBLOCK_SIZE;

}

/* add a superblock to a given heap and sizeclass index and update heap level stats */
void addSuperblockToHeap(cpuheap_t *heap, int sizeClass_ix, superblock_t *pSb){

	size_class_t *pSizeClass=&(heap->_sizeClasses[sizeClass_ix]);

if (!pSizeClass){
	printf("NULL size class!");
	exit(-1);
}

if (!heap){
	printf("NULL heap added!");
	exit(-1);
}
	insertSuperBlock(pSizeClass,pSb);

	/* make this heap the owner*/
	pSb->_meta._pOwnerHeap=heap;

	/* update heap level stats */

	heap->_bytesUsed+=getBytesUsed(pSb);
	heap->_bytesAvailable+=SUPERBLOCK_SIZE;

}

void *allocateBlockFromHeap(cpuheap_t *heap, superblock_t *pSb){


	void *p=allocateFromSuperblock(pSb);
	/* update heap level stats */
	/* 15. u i ← u i + sz. */
	heap->_bytesUsed+=getBlockActualSizeInBytes(pSb->_meta._sizeClassBytes);
	return p;

}

void freeBlockFromHeap(cpuheap_t *pHeap,  block_header_t *pBlock) {
	size_class_t *pSizeClass=getSizeClassForSuperblock(pBlock->_pOwner);
	/* free the block and
	/* 7. s.u ← s.u − block size. */
	freeBlockFromSuperBlock(pBlock->_pOwner,pBlock);
	relocateSuperBlockAhead(pSizeClass,pBlock->_pOwner);
	/*
	 * 6. u i ← u i − block size.
	 */
	pHeap->_bytesUsed-=getBlockActualSizeInBytes(pSizeClass->_sizeClassBytes);

}

/* this is a boolean function to check the condition
 * to transfer superblocks to general heap
 */
char isHeapUnderUtilized(cpuheap_t *pHeap){
	/*
	 * If u i < a i − K ∗ S and u i < (1 − f) ∗ a i,
	 */

	char condition1=(pHeap->_bytesUsed <
			pHeap->_bytesAvailable - HOARD_K * SUPERBLOCK_SIZE );
	char condition2=(pHeap->_bytesUsed <
			(1-HOARD_EMPTY_FRACTION) * pHeap->_bytesAvailable);

	return condition1 && condition2;
}

/* when searching for applicable superblock to transfer to
 * general heap, this function returns the minimal number of
 * free bytes required for the transfered superblock
 * so that the Hoard invariant sustains.
 * This function must only called if isHeapUnderUtilized returns true
 */
size_t getUnderutilizedBytes(cpuheap_t *pHeap) {

	size_t condition1_diff = pHeap->_bytesAvailable - HOARD_K * SUPERBLOCK_SIZE
			- pHeap->_bytesUsed;

	size_t condition2_diff = (1 - HOARD_EMPTY_FRACTION) * pHeap->_bytesAvailable
			- pHeap->_bytesUsed;


	/* return the smaller of the 2 differences */
	return condition1_diff < condition2_diff ? condition1_diff : condition2_diff;

}

superblock_t *findMostlyEmptySuperblock(cpuheap_t *pHeap){
	int i;
	size_t freeBytesRequirement=getUnderutilizedBytes(pHeap);




	for (i=0; i<NUMBER_OF_SIZE_CLASSES; i++){
		superblock_t *p=getLastSuperblockInSizeClass(&(pHeap->_sizeClasses[i]));
		if (!p) /* size class empty */
			continue;
		size_t freeBytes=p->_meta._NoFreeBlks* getBlockActualSizeInBytes(p->_meta._sizeClassBytes);
		if (freeBytes >= freeBytesRequirement)
			return p;
	}

	return NULL;
}
