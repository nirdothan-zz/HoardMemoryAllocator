
#include "mtmm.h"
#include <pthread.h>

typedef struct block_header {
	struct block_header *_pNextBlk;
	struct superblock *_pOwner;
	size_t size;
} block_header_t;

typedef struct  {
	unsigned int _NoBlks, _NoFreeBlks;
	size_t _sizeClassBytes;

	/* Doubly linked list pointers*/
	struct superblock *_pNxtSBlk, *_pPrvSblk;

	struct cpuheap *_pOwnerHeap;

	/*
	 * LIFO stack of free blocks
	 */
	block_header_t *_pFreeBlkStack;

	pthread_mutex_t _sbLock;

} sblk_metadata_t;

/*
 * superblock - allocated on heap
 */
typedef struct superblock {

	sblk_metadata_t _meta;
	/*
	 * actual allocated memory
	 */
	char _buff[SUPERBLOCK_SIZE];

} superblock_t;





/* *convenience structure for list head */
typedef struct {
    unsigned int _length;
    superblock_t  *_first;
} superblock_head_t;



/*
 * collection of superblocks of same size
 * allocated in data segment
 */
typedef struct  {

	/* the size class of objects in superblock list */
	size_t _sizeClassBytes;

	/* Doubly linked list of supoerblocks ordered from the fullest to emptiest
	 * Points to head of list
	 * The members of the list are allocated on the heap
	 * */

	superblock_head_t _SBlkList;

} size_class_t;



/* per CPU memory allocations struct
 * should be allocated in data segment
 */
typedef struct cpuheap{
	unsigned short _CpuId;

	/* u(i) and a(i) from hoard*/
	size_t _bytesUsed, _bytesAvailable;

	size_class_t _sizeClasses[NUMBER_OF_SIZE_CLASSES];

} cpuheap_t;



/* top level memory allocator struct
 * should be allocated in data segment
 */
typedef struct {
	cpuheap_t _heaps[NUMBER_OF_HEAPS+1];

} hoard_t;
