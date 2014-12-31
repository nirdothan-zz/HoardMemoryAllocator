
#include "memory_allocator.h"

#include <pthread.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>



static hoard_t memory;
static pthread_mutex_t heapLocks[NUMBER_OF_HEAPS + 1];
static char isMutexInit;





int getHeapID() {
	int heapid;
	pthread_t self;
	self = pthread_self();
	heapid = (self % 7) % 2;
	heapid++; /* 0 is reserved for general heap so we add 1 */
	return heapid;

}

void initMutexes() {
	int i;
	for (i = 0; i < NUMBER_OF_HEAPS + 1; i++)
		if (pthread_mutex_init(&heapLocks[i], NULL) != 0) {
			printf("\n mutex init failed\n");
			exit(-1);
		}

	isMutexInit = 1; /* so that we won't init again */

}

/*

 The malloc() function allocates size bytes and returns a pointer to the allocated memory.
 The memory is not initialized. If size is 0, then malloc() returns either NULL, or a unique
 pointer value that can later be successfully passed to free().


 malloc (sz)
 1. If sz > S/2, allocate the superblock from the OS and return it.
 2. i ← hash(the current thread).
 3. Lock heap i.
 4. Scan heap i’s list of superblocks from most full to least (for the size class corresponding to sz).
 5. If there is no superblock with free space,
 6. Check heap 0 (the global heap) for a superblock.
 7. If there is none,
 8. Allocate S bytes as superblock s and set the owner to heap i.
 9. Else,
 10. Transfer the superblock s to heap i.
 11. u 0 ← u 0 − s.u
 12. u i ← u i + s.u
 13. a 0 ← a 0 − S
 14. a i ← a i + S
 15. u i ← u i + sz.
 16. s.u ← s.u + sz.
 17. Unlock heap i.
 18. Return a block from the superblock.
 */

void * malloc(size_t sz) {

	int heapIndex, sbIdx, sizeClassIndex;
	superblock_t *pSb;
	void *p;




	/* #1 */
	if (sz > SUPERBLOCK_SIZE / 2) {
		/* in order to identify that this block is large when we free it,
		 * we add a header with the size
		 */

		/* allocate memory to satisfy the large request and overheads*/
		block_header_t *p = getCore(sz + sizeof(block_header_t));
		/* the dummy superblock goes first*/
		p->size = sz;
		p++;
		return (void*) p;
	}

	if (!isMutexInit)
		initMutexes();

	/* #2 */
	heapIndex = getHeapID();



	/* #3 */
	pthread_mutex_lock(&heapLocks[heapIndex]);



	/* #4 */
	sizeClassIndex = getSizeClassIndex(sz);

	/* look in heap i */
	pSb = findAvailableSuperblock(
			&(memory._heaps[heapIndex]._sizeClasses[sizeClassIndex]));
	/* superblock of relevant size class was found in private heap */


	/* #5 && #6 */
	if (!pSb
			&& (pSb =
					findAvailableSuperblock(
							&(memory._heaps[GEREAL_HEAP_IX]._sizeClasses[sizeClassIndex])))) {

		/* superblock of relevant size class was found in general heap
		 * relocate it to private heap step #10
		 */
		//pthread_mutex_unlock(&heapLocks[heapIndex]);
		pthread_mutex_lock(&heapLocks[GEREAL_HEAP_IX]);
		/* #11 #13 */
		removeSuperblockFromHeap(&(memory._heaps[GEREAL_HEAP_IX]),
				sizeClassIndex, pSb);
		pthread_mutex_unlock(&heapLocks[GEREAL_HEAP_IX]);
	//	pthread_mutex_lock(&heapLocks[heapIndex]);
		/* #12 #14 */
		addSuperblockToHeap(&(memory._heaps[heapIndex]), sizeClassIndex, pSb);

	}

	/* #7 */
	if (!pSb) {
		/* superblock of relevant size not found anywhere
		 * generate it
		 */
		pSb = makeSuperblock(pow(2.0, sizeClassIndex));

		/*#8*/
		addSuperblockToHeap(&(memory._heaps[heapIndex]), sizeClassIndex, pSb);


	}

	/* #15, #16 */
	/* this is redundant, but there is no init for heap */
	if (memory._heaps[heapIndex]._CpuId != heapIndex)
		memory._heaps[heapIndex]._CpuId = heapIndex;


	p = allocateBlockFromHeap(&(memory._heaps[heapIndex]), pSb);




	block_header_t *pBlockDEB= getBlockHeaderForPtr(p);

	superblock_t *pSbDEB = pBlockDEB->_pOwner;


		if (!pSbDEB->_meta._pOwnerHeap){

			printf(" NULL superblock owner allocated ");
			printf(" %u  %p  %u\n",pBlockDEB->size,pBlockDEB->_pNextBlk,heapIndex);
			printSizeClass(&(memory._heaps[heapIndex]._sizeClasses[sizeClassIndex] ));
			exit(-1);

		}

		pthread_mutex_unlock(&heapLocks[heapIndex]);

	return p;

}

/*

 The free() function frees the memory space pointed to by ptr, which must have been returned
 by a previous call to malloc(), calloc() or realloc(). Otherwise, or if free(ptr) has already
 been called before, undefined behavior occurs. If ptr is NULL, no operation is performed.


 free (ptr)
 1. If the block is “large”,
 2. Free the superblock to the operating system and return.
 3. Find the superblock s this block comes from and lock it.
 4. Lock heap i, the superblock’s owner.
 5. Deallocate the block from the superblock.
 6. u i ← u i − block size.
 7. s.u ← s.u − block size.
 8. If i = 0, unlock heap i and the superblock and return.
 9. If u i < a i − K ∗ S and u i < (1 − f) ∗ a i,
 10. Transfer a mostly-empty superblock s1
 to heap 0 (the global heap).
 11. u 0 ← u 0 + s1.u, u i ← u i − s1.u
 12. a 0 ← a 0 + S, a i ← a i − S
 13. Unlock heap i and the superblock.
 */
void free(void *ptr) {

	cpuheap_t *pHeap;
	block_header_t *pBlock;


	if (!ptr){
		return;
	}

	pBlock= getBlockHeaderForPtr(ptr);



	/* #1 */
	if (pBlock->size > SUPERBLOCK_SIZE / 2) {
		freeCore((void*) pBlock, (pBlock->size + sizeof(block_header_t)));
		return;
	}

	superblock_t *pSb = pBlock->_pOwner;

	/* #3 */
	pHeap = pSb->_meta._pOwnerHeap;


	/* #4 */
	pthread_mutex_lock(&heapLocks[pHeap->_CpuId]);
	if (!pHeap){

			printf(" NULL superblock owner locked wrong heap");
			printf(" %u  %p \n",pBlock->size,pBlock->_pNextBlk);
			exit(-1);

		}



	/* #5, #6, #7 */
	freeBlockFromHeap(pHeap, pBlock);


	/* #8 */
	if (pHeap->_CpuId == GEREAL_HEAP_IX) {
		pthread_mutex_unlock(&heapLocks[pHeap->_CpuId]);
		return;
	}
	/* #9 */

	if (isHeapUnderUtilized(pHeap)) {
		superblock_t *pSbToRelocate = findMostlyEmptySuperblock(pHeap);


		/* #10 */
		if (pSbToRelocate ) {
			size_t sizeClassIndex = getSizeClassIndex(
					pSbToRelocate->_meta._sizeClassBytes);


			/* #11 #12 */
			removeSuperblockFromHeap(pHeap, sizeClassIndex, pSbToRelocate);
		//	pthread_mutex_unlock(&heapLocks[pHeap->_CpuId]);
			/* pthread_mutex_unlock(&heapLocks[pHeap->_CpuId]);			 */
			pthread_mutex_lock(&heapLocks[GEREAL_HEAP_IX]);
			/* #11 #12 */

			addSuperblockToHeap(&(memory._heaps[GEREAL_HEAP_IX]),
					sizeClassIndex, pSbToRelocate);
			memory._heaps[GEREAL_HEAP_IX]._CpuId=0;
	    	pthread_mutex_unlock(&heapLocks[GEREAL_HEAP_IX]);

		} else {
	//		pthread_mutex_unlock(&heapLocks[pHeap->_CpuId]);
		}
	}

	/* #13 */
	pthread_mutex_unlock(&heapLocks[pHeap->_CpuId]);
	return;

}

void *calloc(size_t nmemb, size_t size) {
	return malloc(nmemb * size);
}
/*
 1. allocate sz bytes
 2. copy from old location to a new one
 3. free old allocation
 */
void *realloc(void *ptr, size_t sz) {
	void *p = malloc(sz);
	if (!p) {
		perror("realloc failed\n");
		return NULL;
	}
	if (!ptr)
		return p;

	if (!sz){
		free(ptr);
		return NULL;
	}
	block_header_t *pHeader = ptr - sizeof(block_header_t);
	unsigned int size = pHeader->size < sz ? pHeader->size : sz;

	memcpy(p, ptr, size);
	free(ptr);
	return p;
}

