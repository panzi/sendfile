CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Werror -std=c11 -O2 -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE
OBJS=sendfile.o parse.o

.PHONY: all clean

all: sendfile

sendfile: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

sendfile.o: sendfile.c parse.h
	$(CC) $(CFLAGS) -c -o $@  $<

parse.o: parse.c parse.h
	$(CC) $(CFLAGS) -c -o $@  $<

clean:
	rm -r sendfile.o parse.o sendfile
