/*
 * meory_allocator.h
 *
 *  Created on: Dec 28, 2014
 *      Author: student
 */

#include "structs.h"

#ifndef MEORY_ALLOCATOR_H_
#define MEORY_ALLOCATOR_H_

size_t getBlockActualSizeInHeaders(size_t sizeClassBytes);
size_t getBlockActualSizeInBytes(size_t sizeClassBytes);

void *getCore(size_t size);
void freeCore(void *p, size_t length);
void closeMmap();
superblock_t* makeSuperblock(size_t sizeClassBytes);
block_header_t *popBlock(superblock_t *pSb);
superblock_t *pushBlock(superblock_t *pSb, block_header_t *pBlk);
unsigned short getFullness(superblock_t *pSb);
void printSuperblock(superblock_t *pSb);
size_t getBytesUsed(const superblock_t *pSb);
block_header_t *getBlockHeaderForPtr(void *ptr) ;
superblock_t *getSuperblockForPtr(void *ptr)  ;
superblock_t* makeDummySuperblock(superblock_t *pSb, size_t sizeClassBytes) ;
void freeBlockFromSuperBlock(superblock_t *pSb, block_header_t *pBlock) ;
void removeSuperblockFromHeap(cpuheap_t *heap, int sizeClass_ix, superblock_t *pSb);
void addSuperblockToHeap(cpuheap_t *heap, int sizeClass_ix, superblock_t *pSb);
void *allocateBlockFromHeap(cpuheap_t *heap, superblock_t *pSb);
void freeBlockFromHeap(cpuheap_t *pHeap,  block_header_t *pBlock);
char isHeapUnderUtilized(cpuheap_t *pHeap);
size_t getUnderutilizedBytes(cpuheap_t *pHeap);
superblock_t *findMostlyEmptySuperblock(cpuheap_t *pHeap);

void plantSuperBlock(superblock_t *pBefore, superblock_t *pNode,superblock_t *pAfter);
void insertSuperBlock(size_class_t *sizeClass, superblock_t *superBlock);
superblock_t *findAvailableSuperblock(size_class_t *sizeClass);
superblock_t *popLastSuperblock(size_class_t *sizeClass);
void relocateSuperBlockAhead(size_class_t *sizeClass, superblock_t *superBlock);
void relocateSuperBlockBack(size_class_t *sizeClass, superblock_t *superBlock);
superblock_t *removeSuperblock(size_class_t *sizeClass,	superblock_t *superBlock);
void printSizeClass(size_class_t *sizeClass);
superblock_t *getLastSuperblockInSizeClass(size_class_t *pSizeClass);
size_t getSizeClassIndex(size_t size);
size_class_t *getSizeClassForSuperblock(superblock_t *pSb);






#endif /* MEORY_ALLOCATOR_H_ */
