#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAPFILE "/dev/zero"

static int fd;

void *getCore(unsigned int size) {

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
	return p;
}


void closeMmap(){
	close(fd);
}
