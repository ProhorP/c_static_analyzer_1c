CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -g
GLIB_CONF=`pkg-config --cflags glib-2.0`
GLIB_LIB=`pkg-config --libs glib-2.0`
PCRE_LIB=`pcre-config --libs`

all: test

test: make_splitter build_test test01 test02 test03 clean
memory_leak_test: make_splitter build_test test01_m test02_m test03_m clean

make_splitter:
	~/re2c/.build/re2c ./splitter_template.c -o ./splitter.c -8 --case-ranges -i -I ~/re2c/include/

build_test: splitter.c
	$(CC) $(CFLAGS) $(GLIB_CONF) splitter.c $(GLIB_LIB) $(PCRE_LIB)

test01:
	./a.out ./test/test01 ./log

test02:
	./a.out ./test/Module.bsl ./log

test03:
	./a.out ./test/test03 ./log

test01_m:
	valgrind --leak-check=full ./a.out ./test/test01 ./log

test02_m:
	valgrind --leak-check=full ./a.out ./test/Module.bsl ./log

test03_m:
	valgrind --leak-check=full ./a.out ./test/test03 ./log

clean:
	rm a.out; rm splitter.c 

log_clean:
	rm ./log 
