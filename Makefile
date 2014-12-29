CC=gcc

TARGET = linux-scalability

MYFLAGS =  -g -O0 -Wall -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free

# uncomment this to link with hoard memory allicator
MYLIBS = libmtmm.a


all: $(TARGET) $(MYLIBS)
#all: libSimpleMTMM.a $(TARGET)


libmtmm.a:
	$(CC) $(MYFLAGS) -c core_memory_allocator.c cpu_heap.c memory_allocator.c size_class.c superblock.c
	ar rcu libmtmm.a core_memory_allocator.o cpu_heap.o memory_allocator.o size_class.o superblock.o
	ranlib libmtmm.a


$(TARGET): $(TARGET).c libmtmm.a
	$(CC) $(CCFLAGS) $(MYFLAGS) $(TARGET).c libmtmm.a -o $(TARGET) -lpthread -lm

clean:
	rm -f $(TARGET)  *.o  libmtmm.a