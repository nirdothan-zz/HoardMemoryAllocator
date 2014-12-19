CC=gcc

TARGET = linux-scalability

MYFLAGS =  -g -O0 -Wall -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free 
MYLIBS = libmtmm.a

$(TARGET): $(TARGET).c
	$(CC) $(CCFLAGS) $(MYFLAGS) $(MYLIBS) $(TARGET).c -o $(TARGET) -lpthread -lm

clean:
	rm -f $(TARGET) $(MYLIBS) *.o
