CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -g
GLIB_CONF=`pkg-config --cflags glib-2.0`
GLIB_LIB=`pkg-config --libs glib-2.0`
PCRE_LIB=`pcre-config --libs`

all: build_test test01 test02 test03 clean

build_test: splitter.c
	$(CC) $(CFLAGS) $(GLIB_CONF) splitter.c $(GLIB_LIB) $(PCRE_LIB)

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
