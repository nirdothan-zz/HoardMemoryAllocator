#include "mtmm.h"

typedef struct header {
	struct header *_pNextBlk;
	struct superblock *_pOwner;
} block_header_t;

typedef struct sblk_metadata {
	unsigned int _NoBlks, _NoFreeBlks;

	/* Doubly linked list pointers*/
	struct superblock *_pNxtSBlk, *_pPrvSblk;
	/*
	 * LIFO stack of free blocks
	 */
	block_header_t *_pFreeBlkStack;

} sblk_metadata_t;

/*
 * superblock - allocated on heap
 */
typedef struct superblock {

	sblk_metadata_t _meta;
	/*
	 * actual allocated memory
	 */
	char _buff[SUPERBLOCK_SIZE - sizeof(sblk_metadata_t)];

} superblock_t;

/*
 * collection of superblocks of same size
 * allocated in data segment
 */
typedef struct {

	/* the size class of objects in superblock list */
	unsigned int _sizeClass;

	/* Doubly linked list of supoerblocks ordered from the fullest to emptiest
	 * Points to head of list
	 * The members of the list are allocated on the heap
	 * */

	superblock_t *_pSBlkList;

} size_class_t;

/* per CPU memory allocations struct
 * should be allocated in data segment
 */
typedef struct {
	unsigned short _CpuId;
	size_class_t _sizeClasses[16];

} cpuheap_t;

/* top level memory allocator struct
 * should be allocated in data segment
 */
typedef struct {
	cpuheap_t _heaps[3];

} hoard_t;
