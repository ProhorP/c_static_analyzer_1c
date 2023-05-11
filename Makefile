CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -g

all: build_test test01 test02 test03 clean

build_test: splitter.c
	$(CC) $(CFLAGS) splitter.c

test01:
	./a.out ./test/test01 ./log

test02:
	./a.out ./test/test02 ./log

test03:
	./a.out ./test/test03 ./log

clean:
	rm a.out 

clean_log:
	rm ./log 
