CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -ggdb

all: main.o
	
main.o: main.c opcodes.c defines.h
	$(CC) $(CFLAGS) main.c opcodes.c -o main
