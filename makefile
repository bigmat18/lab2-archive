SRC_DIR = src
CC = gcc
OBJ_NAME = archivio
COMPILER_FLAGS = -std=c11 -Wall -O0 -g

all: archivio client1 client2

archivio:
	$(CC) $(COMPILER_FLAGS) archivio.c utils.c -o archivio

client2:
	$(CC) $(COMPILER_FLAGS) client2.c utils.c -o client2