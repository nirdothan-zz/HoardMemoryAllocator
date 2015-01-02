/*
 * core_memory_allocator.c
 *
 *  Created on: Dec 18, 2014
 *      Author: Nir Dothan 028434009
 *
 *      This module allocates and free memory from the system by mapping and unmapping annonymus files to virtual memory
 */

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define MAPFILE "/dev/zero"



void *getCore(size_t size) {
	int fd;

	fd = open(MAPFILE, O_RDWR);

   if (fd == -1) {
		perror("Error opening file for writing");
		exit(-1);
    }

	void *p = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(-1);
    }
    close(fd);
	return p;
}

void freeCore(void *p, size_t length){

	 if (munmap(p, length) == -1) {

			perror("Error freeing mapped  memory");
			exit(-1);
	   }
}

