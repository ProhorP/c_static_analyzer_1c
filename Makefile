CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -g
GLIB_CONF=`pkg-config --cflags glib-2.0`
GLIB_LIB=`pkg-config --libs glib-2.0`
PCRE_LIB=`pcre-config --libs`

all: test

test: generic_lex build_test test01 test02 test03 

memory_leak_test: generic_lex build_test test01_m test02_m test03_m 

analyzer_clang:
	scan-build make

pvs_studio_analyzer:
	pvs-studio-analyzer trace -o /tmp/strace_out make;
	pvs-studio-analyzer analyze -f /tmp/strace_out -l /home/user/.config/PVS-Studio/PVS-Studio.lic -o /tmp/PVS-Studio.log;
	plog-converter -a GA:1,2 -t tasklist -o /tmp/report.tasks /tmp/PVS-Studio.log;
	vim /tmp/report.tasks

generic_lex:
	re2c ./lex.l -o ./lex.c -8 --case-ranges -i

build_test:
	$(CC) $(CFLAGS) $(GLIB_CONF) ./tests/lex_test/main.c lex.c print_error.c $(GLIB_LIB) $(PCRE_LIB) -o ./tests/lex_test/test

test01:
	./tests/lex_test/test ./tests/lex_test/module01.bsl ./tests/lex_test/log

test01_m:
	valgrind --leak-check=full ./tests/lex_test/test ./tests/lex_test/module01.bsl ./tests/lex_test/log

test02:
	./tests/lex_test/test ./tests/lex_test/module02.bsl ./tests/lex_test/log

test02_m:
	valgrind --leak-check=full ./tests/lex_test/test ./tests/lex_test/module02.bsl ./tests/lex_test/log

test03:
	./tests/lex_test/test ./tests/lex_test/module03.bsl ./tests/lex_test/log

test03_m:
	valgrind --leak-check=full ./tests/lex_test/test ./tests/lex_test/module03.bsl ./tests/lex_test/log

clean:
	rm ./tests/lex_test/test;
	rm lex.c 
	rm ./tests/lex_test/log 
