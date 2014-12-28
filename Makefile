CC :- gcc
program_NAME := main
program_C_SRCS := $(wildcard *.c)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_OBJS := $(program_C_OBJS)
program_LIBRARY_DIRS :=
program_LIBRARIES :=
MY_LIBRARIES := libmtmm.a
MYFLAGS =  -g -O0 -Wall -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free

LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all clean distclean

all: $(program_NAME) 


$(program_NAME): $(program_OBJS) libmtmm.a
	$(CC) $(CCFLAGS) $(MYFLAGS) libmtmm.a -o $(program_NAME) -lpthread -lm

libmtmm.a:  $(program_OBJS)
	$(CC) $(MYFLAGS) -c memory_allocator.c -lpthread -lm
	ar rcu libmtmm.a  $(program_OBJS)
	ranlib libmtmm.a


clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)
	@- $(RM) $(MY_LIBRARIES)

distclean: clean
