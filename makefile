CC=gcc
CFLAGS=-g -Wall -Wextra -O -pthread
LDLIBS=-lm -pthread

EXECS=archivio client1 client2

all: $(EXECS) 



archivio: archivio.o hash_table.o buffer.o thread.o connection.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

archivio.o: archivio.c hash_table.h buffer.h thread.h connection.h
		$(CC) $(CFLAGS) -c $<



client1: client1.o connection.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

client1.o: client1.c connection.h
		$(CC) $(CFLAGS) -c $<



client2: client2.o thread.o connection.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

client2.o: client2.c thread.h connection.h
		$(CC) $(CFLAGS) -c $<



clean_all: 
	rm -f *.o *.out $(EXECS) capolet caposc lettori.log server.log valgrind-*.log

clean: 
	rm capolet caposc lettori.log server.log valgrind-*.log

