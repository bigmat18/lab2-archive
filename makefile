CC=gcc
CFLAGS=-g -Wall -Wextra -O -pthread
LDLIBS=-lm -pthread

EXECS=archivio client1 client2

all: $(EXECS) 



archivio: archivio.o hash_table.o buffer.o thread.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

archivio.o: archivio.c hash_table.h buffer.h thread.h
		$(CC) $(CFLAGS) -c $<



client1: client1.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)



client2: client2.o thread.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

client2.o: client2.c thread.h
		$(CC) $(CFLAGS) -c $<



clean: 
	rm -f *.o *.out $(EXECS) capolet caposc lettori.log server.log valgrind-*.log

