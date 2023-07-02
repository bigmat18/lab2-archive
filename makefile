CC=gcc
CFLAGS=-std=c11 -Wall -g -O -pthread
LDLIBS=-lm -pthread

EXECS=archivio.out client1.out client2.out

all: $(EXECS) 



archivio.out: archivio.o utils.o hash_table.o buffer.o thread.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

archivio.o: archivio.c utils.h hash_table.h buffer.h thread.h
		$(CC) $(CFLAGS) -c $<



client1.out: client1.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)



client2.out: client2.o utils.o
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

client2.o: client2.c utils.h
		$(CC) $(CFLAGS) -c $<



clean: 
	rm -f *.o $(EXECS)

