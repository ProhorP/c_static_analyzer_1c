CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -g

all: build_test test01 clean

build_test: splitter.c
	$(CC) $(CFLAGS) splitter.c

test01:
	./a.out ./test/test01

clean:
	rm a.out 
